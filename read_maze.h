#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "previous_rpg.h"

#define m_constant_health_points 10

int play_maze();
void create_maze();
void update_maze();

typedef struct {
  bool character_created;
  bool saved_game;
} initial_game_state;

typedef struct {
  int race;                                 // 1 - Human; 2 - Dwarf; 3 - Elf
  int alignment;                            // 1 - Evil; 2 - Neutral; 3 - Good
  int job;                                  // 1 - Warrior; 2 - Wizard; 3 - Thief
  char background_story[400];               
  int size;                                 // 1 - Big; 2 - Medium; 3 - Small
  int strength;                             // [1, 5] 
  int constitution;                         // [1, 5]   
  int dexterity;                            // [1, 5]
  int inteligence;                          // [1, 5]
  int health_points;
} character_type;

// this function returns true if a number n is inside
// the range [minimun, maximun]
bool is_inside_of_range(int minimun, int maximun, int n) {
  if (n >= minimun && n <= maximun) 
    return true;
  return false;
}

// the following functions print the dialog box of the
// name associated functions
void set_race_dialog_box() {
  printf("Escolha a raça do seu personagem:\n"
	 "1 - Humano\n"
	 "2 - Anão\n"
	 "3 - Elfo\n");
};
void set_alignment_dialog_box() {
  printf("Escolha o alimenhamento do seu personagem:\n"
	 "1 - Mal\n"
	 "2 - Neutro\n"
	 "3 - Bom\n");
};
void set_job_dialog_box() {
  printf("Escolha a profissão do seu personagem:\n"
	 "1 - Guerreiro\n"
	 "2 - Mago\n"
	 "3 - Ladino\n");
};
void set_background_story_dialog_box() {
  printf("História Prévia: Conte como o personagem viveu até aquele momento\n"
	 "Texto de 400 characteres no máximo\n");
};
void set_size_dialog_box() {
  printf("Escolha o tamanho do seu personagem:\n"
	 "1 - Grande\n"
	 "2 - Médio\n"
	 "3 - Pequeno\n");
};
void set_attributes_dialog_box() {
  printf("\nA soma dos atributos força, constituição, destreza e inteligência,"
	 " não pode ser maior do que 10.\n"
	 "Os ataques são associados a classe do personagem:\n"
	 "O guerreiro ataca usando força.\n"
	 "O mago ataca usando inteligência.\n"
	 "E o ladino ataca usando destreza.\n\n");
}
void set_strength_dialog_box() {
  printf("Escolha a força do seu personagem:\n"
	 "Um inteiro de 1 até 5\n");
};
void set_constitution_dialog_box() {
  printf("Escolha a constituição do seu personagem:\n"
	 "Um inteiro de 1 até 5\n");
};
void set_dexterity_dialog_box() {
  printf("Escolha a destreza do seu personagem:\n"
	 "Um inteiro de 1 até 5\n");
};
void set_inteligence_dialog_box() {
  printf("Escolha a inteligência do seu personagem:\n"
	 "Um inteiro de 1 até 5\n");
}
// this function implements the first restriction
// if the player choosed the Dwarf Race, the
// Big size is not available
bool set_character_restriction_1(character_type * player, int choice) {
  if (player->race == 2 && choice == 1) {
    printf("\nVocê não pode escolher a raça Anão e o tamanho"
	   " Grande\n\n");
    return true;
  } else {
    return false;
  }
}
// this function implements the second restriction
// if the player choosed the good alignment
// then he cannot choose the thief job
bool set_character_restriction_2(character_type * player, int choice) {
  if (player->alignment == 3 && choice == 3) {
    printf("\nVocê não pode escolher a profissão ladino para um "
	   "jogador de alinhamento bom.\n \n");
    return true;
  } else {
    return false;
  }
}
// this function implements the third restriction
// the sum of player strength, constitution,
// dexterity and inteligence cannot surpass the
// total of 10 points
bool set_character_restriction_3(character_type * player, int choice, int max) {
  int total_sum = 0;
  total_sum = player->strength + player->constitution + player->dexterity + player->inteligence + choice;
  if (total_sum > max) {
    printf("\nVocê ultrapassou/ultrapassará o limite de 10 pontos.\n\n");
    return true;
  }
  return false;
}
// this function implements the 4th restriction
// when the player chooses the dwarf race
// +1 strength -1 dexterity
void set_character_restriction_4(character_type * player) {
  if (player->race == 2) {
    player->strength++;
    player->dexterity--;
  }
}
// this function implements the 5th restriction
// when the player chooses the elf race
// +1 dexterity -1 constitution
void set_character_restriction_5(character_type * player) {
  if (player->race == 3) {
    player->dexterity++;
    player->constitution--;
  }
}
// this function implements the 7th restriction
// It makes health points according to the formula
// constitution * m_constant_health_points
// if the constitution = 0, the health points
// should be equal to 1
void set_character_hp(character_type * player) {
  if (player->constitution == 0) {
    player->health_points = 1;
  } else {
    player->health_points = player->constitution * m_constant_health_points;
  }
}
// this function changes the race of the player
void set_character_race(character_type * player) {
  int race;
  do
    {
      set_race_dialog_box();
      scanf("%d", &race);
    } while (!is_inside_of_range(1, 3, race));
  player->race = race;
};
// this function sets the aligment of the player
void set_character_alignment(character_type * player) {
  int alignment;
  do
    {
      set_alignment_dialog_box();
      scanf("%d", &alignment);
    } while (!is_inside_of_range(1, 3, alignment));
  player->alignment = alignment;
}
// this function sets the job of the player
// if the charcter has choosed 3 - good as
// alignment - the thief job is not available
void set_character_job(character_type * player) {
  int job;
  bool flag;
  do
    {
      set_job_dialog_box();
      scanf("%d", &job);
      flag = set_character_restriction_2(player, job);
    } while (!is_inside_of_range(1, 3, job) || flag);
  player->job = job;
}
// this function sets the background story of the character
void set_character_background(character_type * player) {
  set_background_story_dialog_box();
  scanf(" %[^\n]s", player->background_story);
}
// this funtion sets the character size
void set_character_size(character_type * player) {
  int size;
  bool flag;
  do
    {
      set_size_dialog_box();
      scanf("%d", &size);
      flag = set_character_restriction_1(player, size);
    } while (!is_inside_of_range(1, 3, size) || flag);
  player->size = size;
}
// this function sets the player attributes to zero
void set_to_zero_player_attributes(character_type * player) {
  player->strength = 0;
  player->constitution = 0;
  player->dexterity = 0;
  player->inteligence = 0;
}
// this function sets the character strength
void set_character_strength(character_type * player) {
  int strength;
  do
    {
      set_strength_dialog_box();
      scanf("%d", &strength);
    } while (!is_inside_of_range(1, 5, strength) ||
	     set_character_restriction_3(player, strength, 10));
  player->strength = strength;
}
// this function sets the character constitution
void set_character_constitution(character_type * player) {
  int constitution;
  do
    {
      set_constitution_dialog_box();
      scanf("%d", &constitution);
    } while (!is_inside_of_range(1, 5, constitution) ||
	     set_character_restriction_3(player, constitution, 8));
  player->constitution = constitution;
}
// this function sets the character inteligence
void set_character_inteligence(character_type * player) {
  int inteligence;
  do
    {
      set_inteligence_dialog_box();
      scanf("%d", &inteligence);
    } while (!is_inside_of_range(1, 5, inteligence) ||
	     set_character_restriction_3(player, inteligence, 10));
  player->inteligence = inteligence;
}
// this function sets the character dexterity
void set_character_dexterity(character_type * player) {
  int dexterity;
  do
    {
      set_dexterity_dialog_box();
      scanf("%d", &dexterity);
    } while (!is_inside_of_range(1, 5, dexterity) ||
	     set_character_restriction_3(player, dexterity, 9));
  player->dexterity = dexterity;
}

// the following functions print an atributte of the character
void print_character_race(character_type player) {
  int choice = player.race;
  switch (choice) {
  case 1: {
    printf("Humano\n");
    break;
  }
  case 2: {
    printf("Anão\n");
    break;
  }
  case 3:
    printf("Elfo\n");
default:
    break;
  }
}
void print_character_alignment(character_type player) {
  int choice = player.alignment;
  switch (choice) {
  case 1: {
    printf("Mal\n");
    break;
  }
  case 2: {
    printf("Neutro\n");
    break;
  }
  case 3:
    printf("Bom\n");
default:
    break;
  }
}
void print_character_job(character_type player) {
  int choice = player.job;
  switch (choice) {
  case 1: {
    printf("Guerreiro\n");
    break;
  }
  case 2: {
    printf("Mago\n");
    break;
  }
  case 3:
    printf("Ladino\n");
default:
    break;
  }
}
void print_character_size(character_type player) {
  int choice = player.size;
  switch (choice) {
  case 1: {
    printf("Grande\n");
    break;
  }
  case 2: {
    printf("Médio\n");
    break;
  }
  case 3:
    printf("Pequeno\n");
default:
    break;
  }
}
void print_character_other(character_type player) {
  printf("Força:          %d\n", player.strength);
  printf("Constituição:   %d\n", player.constitution);
  printf("Destreza:       %d\n", player.dexterity);
  printf("Inteligência:   %d\n", player.inteligence);
  printf("Pontos de vida: %d\n", player.health_points);
}
// this function prints all of the character attributtes
void print_character_attributtes(character_type player) {
  printf("\nPersonagem:\n");
  printf("Raça:           ");
  print_character_race(player);
  printf("Alinhamento:    ");
  print_character_alignment(player);
  printf("Profissão:      ");
  print_character_job(player);
  printf("Porte:          ");
  print_character_size(player);
  print_character_other(player);
  printf("\n");
}
// this function sets the characteristics 
// of the character
character_type set_character_attributtes() {
  character_type player;
  set_character_race(&player);
  set_character_alignment(&player);
  set_character_job(&player);
  set_character_background(&player);
  set_character_size(&player);
  
  set_attributes_dialog_box();
  set_to_zero_player_attributes(&player);
  
  set_character_strength(&player);
  set_character_constitution(&player);
  set_character_dexterity(&player);
  set_character_inteligence(&player);
  
  set_character_restriction_4(&player);
  set_character_restriction_5(&player);
  set_character_hp(&player);

  print_character_attributtes(player);
  
  return player;
};

// this function displays the choices of the
// player in the entry menu
void entry_menu_dialog_box() {
  printf("1. Criar Personagem\n"
	 "2. Iniciar Jogo\n"
	 "3. Carregar Jogo\n"
	 "4. Criar um Labirinto\n"
	 "5. Atualizar um Labirinto\n"
	 "6. Sair\n");
}


void initialize_game(character_type player_character) {
  
};
void get_saved_game() {
  
};

// this function acts as the entry menu
// of the game. It returns 0 if the player
// chooses to leave the game.
int entry_menu() {
  int choice;
  initial_game_state game_state;
  game_state.character_created = false;
  game_state.saved_game = false;
  bool repeat = true;
  character_type player_character;
  
  do
    {
      entry_menu_dialog_box();
      scanf("%d", &choice);
      // if the number typed by the player
      // is outside of the range of options
      // repeat the question.
      if (!is_inside_of_range) {
	// if the player chooses the 1st option
	// run the create_character_function
      } else if (choice == 1) {
	player_character = set_character_attributtes();
	game_state.character_created = true;
	// if the player chooses the 2nd option
	// and if he has created the character
	// initialize the game
	// else print an error message and ask the question again
      } else if (choice == 2) {
	if (game_state.character_created == true) {
	  initialize_game(player_character);
	  printf("Game initialized\n");
	  play_maze(player_character);
	} else {
	  printf("Você precisa criar um personagem antes de iniciar o jogo\n");
	}
	// if the player chooses the 3rd option
	// load the previous game.
	// If the player doesn't have a saved game print an error
	// message and ask again
      } else if (choice == 3) {
	if(game_state.saved_game == true) {
	  get_saved_game();
	} else {
	  printf("Você não tem um jogo salvo\n");
	}
	// if the player chooses the 4th option
	// exit the game
      } else if (choice == 6) {
	printf("oK\n");
	repeat = false;
      }
      else if (choice == 4) {
	create_maze();
      }
      else if (choice == 5) {
	update_maze();
      }
    } while (repeat);
  
  return 0;
}

#define STATUS_DURATION 20

typedef struct {
  int strength;                             // [1, 5] 
  int constitution;                         // [1, 5]   
  int dexterity;                            // [1, 5]
  int inteligence;                          // [1, 5]
  int health_points;
} monster_type;


// this function return the absolute of an integer
int abs(int a) {
  if (a > 0) return a;
  else return -a;
}
// this type caracterizes a position on the matrix
typedef struct {
  int x;
  int y;
} position;

// this function moves a position to the right
void right(position * pos) {
  pos->y++;
}
// this function moves a position to the left
void left(position * pos) {
  pos->y--;
}
// this function moves a position down
void down(position * pos) {
  pos->x++;
}
// this function moves a position up
void up(position * pos) {
  pos->x--;
}
// this function alters the display to red
void red() {
  printf("\033[1;31m");
}
// this function alters the display to blue
void blue() {
  printf("\033[1;34m");
}
// this function alters the display to yellow
void yellow() {
  printf("\033[0;33m");
}
// this function alters the display to green
void green() {
  printf("\033[0;32m");
}
// this function alters the display to purple
void purple() {
  printf("\033[1;35m");
}
// this functions resets the display
void reset() {
  printf("\033[0m");
}
// this function prints a wall
void print_wall() {
  yellow();
  printf("@");
  reset();
  printf(";");
}
// this function prints the player
void print_player() {
  blue();
  printf("PE");
}
// this functions prints an empty space in the maze
void print_empty() {
  reset();
  printf("  ");
}
// this funtion prints a trap in the maze
void print_trap() {
  red();
  printf("A;");
}
// this function prints a heath potion in the maze
void print_health_potion() {
  green();
  printf("hp");
}
// this function prints a potencializer
void print_potenzializer() {
  purple();
  printf("pp");
}
// this function prints the final boss
void print_boss() {
  red();
  printf("BB");
}
// this function prints the whole matrix
void print_matrix(int size, int array[][size]) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (array[i][j] ==  1) print_wall();
      else if (array[i][j] ==  4) print_potenzializer();
      else if (array[i][j] ==  3) print_health_potion();
      else if (array[i][j] ==  2) print_trap();
      else if (array[i][j] == -1) print_player();
      else if (array[i][j] ==  7) print_boss();
      else print_empty();
    }
    printf("\n");
  }
}
// this function gets the position of the player and
// another positioin on the maze. If the position is outside
// of the player range - it returns false
bool is_outside_of_range(position *pos, int i, int j) {
  if (abs(pos->y - j) > 2 || abs(pos->x - i) > 2) return true;
  return false;
}
// this function decides if the trap would be shown to the player
bool show_trap(character_type *player) {
  float probability = player->dexterity + player->inteligence;
  if (player->job == 3) probability /= 12;
  else probability /= 20;
  probability *= 100;
  int random_int = rand() % 100;
  if (probability >= random_int) return true;
  return false;
}
// this function prints the positions in the player range
void print_close_matrix(int size, void *array0, position *pos, character_type *player) {
  int (*arr)[size] = array0;
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (is_outside_of_range(pos, i, j)) print_empty();
      else if (arr[i][j] ==  4) print_potenzializer();
      else if (arr[i][j] ==  3) print_health_potion();
      else if (arr[i][j] ==  2) {
	if (show_trap(player)) {
	  print_trap();
	  arr[i][j] = 5;
	} else print_empty();
      }
      else if (arr[i][j] ==  5) print_trap();
      else if (arr[i][j] ==  1) print_wall();
      else if (arr[i][j] ==  0) print_empty();
      else if (arr[i][j] == -1) print_player();
      else if (arr[i][j] ==  7) print_boss();
      }
    printf("\n");
  }
}
// this function prints the player stats
void print_player_stats(character_type * player) {
  printf("HP: %d\n", player->health_points);
  printf("Inteligence:  %d\n", player->inteligence);
  printf("Constitution: %d\n", player->constitution);
  printf("Dexterity:    %d\n", player->dexterity);
  printf("Strength:     %d\n", player->strength);
}


// this function puts a -1 in the new player position
void change_position(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] = array0;
  array[pos->x][pos->y] = -1;
}
// this function puts a 0 in the array from the given position 
void clear_position(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] = array0;
  array[pos->x][pos->y] = 0;
}
// this function puts a 1 in the array from the given position
void put_wall(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] = array0;
  array[pos->x][pos->y] = 1;
}
// this function gets the item and atualizes the player stats with the new item
void change_player_stats_from_position(character_type *player, int item, int *stats_turns) {
  if (item == 2 || item == 5) player->health_points -= 1;
  else if (item == 3) player->health_points += player->constitution;
  else if (item == 7) resultado_partida();
  else if (item == 4) {
    *stats_turns += STATUS_DURATION;
    if (player->job == 1) player->strength *= 2;
    else if (player->job == 2) player->inteligence *= 2;
    else if (player->job == 3) player->dexterity *= 2;
  }
}
// this function backs the stats of the player to normal
void back_to_normal_stats(character_type *player, int *stats_turns) {
  if (*stats_turns == 1) {
    *stats_turns -= 1;
    if (player->job == 1) player->strength /= 2;
    else if (player->job == 2) player->inteligence /= 2;
    else if (player->job == 3) player->dexterity /= 2;
  }
  else if (*stats_turns > 0) *stats_turns -= 1;
}
// this function changes the player position and clears the old one
void change_player_to_direction(void *array0, int SIZE, position *pos, char direction) {
  int (*array)[SIZE] = array0;
  clear_position(array, SIZE, pos);
  if      (direction == 'w') up(pos);
  else if (direction == 'd') right(pos);
  else if (direction == 's') down(pos);
  else if (direction == 'a') left(pos);
  //change_position(array, SIZE, pos);
}


// this function verifies if the right of the player is free
bool is_right_free(void *array0, int SIZE,  position *pos) {
  int (*array)[SIZE] = array0;
  if (pos->y == SIZE - 1) return false;
  else if (array[pos->x][pos->y + 1] == 1) return false;
  return true;
}
// this function verifies if the left of the player is free
bool is_left_free(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] = array0;
  if(pos->y == 0) return false;
  else if (array[pos->x][pos->y - 1] == 1) return false;
  return true;
}
// this function verifies if up is free
bool is_up_free(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] =array0;
  if (pos->x == 1 || pos->x == 0) return false;
  else if (array[pos->x - 1][pos->y] == 1) return false;
  return true;
}
// this function verifies if down is free
bool is_down_free(void *array0, int SIZE, position *pos) {
  int (*array)[SIZE] = array0;
  if (pos->x == SIZE - 1) return false;
  else if (array[pos->x + 1][pos->y] == 1) return false;
  return true;
}
// this function verifies if a direction is free
bool is_direction_free(void *array0, int SIZE, position *pos, char direction) {
  int (*array)[SIZE] = array0;
  if      (direction == 'w') is_up_free(array, SIZE, pos);
  else if (direction == 'd') is_right_free(array, SIZE, pos);
  else if (direction == 'a') is_left_free(array, SIZE, pos);
  else if (direction == 's') is_down_free(array, SIZE, pos);
}

// this function moves the player to a certain
// direction if this direction is free
void move_player(void *array0, int SIZE, position *old_position, char direction, character_type *player) {
  int (*array)[SIZE] = array0;
  if (is_direction_free(array, SIZE, old_position, direction)) {
    change_player_to_direction(array, SIZE, old_position, direction);
   }
  //else printf("direction not free\n");;
}


// Função de captura de entrada do teclado
int getch(void) {
  int c = 0;
  struct termios org_opts, new_opts;
  int res = 0;
  // guarda configurações antigas
  res = tcgetattr(STDIN_FILENO, &org_opts);
  assert(res==0);
  // configura novos parametros de entrada
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT |
			ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c = getchar();
  // restore old settings
  res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res==0);
  return (c);
}

/*
char str_name[40];
printf("Which maze file do you want to open?\n");
scanf(" %s", str_name);
/*/

// this function gets a maze from a txt file 
void get_array_from_file(char * str_name, int N, int array[][N])
{
  
  FILE *p_file = fopen(str_name, "r");

  if (p_file == NULL) {
    printf("Error, file not open\n");
    exit(1);
  }
  
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      fscanf(p_file, "%d", &array[i][j]);
    }
  }
  fclose(p_file);

}

// this function gets the difficulty of the game
int get_difficulty() {
  int answer;
  printf("Dificuldade:\n");
  printf("1 - Fácil\n");
  printf("2 - Médio\n");
  printf("3 - Difícil\n");
  do
    {
      scanf("%d", &answer);
    } while (answer < 1 || answer > 3);
  return answer;
}

// this funtion returns a random number between the boundaries, including
int random_integer(int smaller, int bigger) {
  srand(time(NULL));
  return  rand() % (++bigger - smaller) + smaller;
}

void name_of_the_maze_file(int size, int choice, char str_name[12]) {

  char str_choice = choice + 48;

  strcpy(str_name, "mz_");
    
  if      (size == 30) strcat(str_name, "30_");
  else if (size == 60) strcat(str_name, "60_");
  else if (size == 90) strcat(str_name, "90_");
  
  str_name[6] = str_choice;

  strcat(str_name, ".txt\0");

}

// this functions are from the maze creator
// this function verifies if the right of the player is free
bool is_right_free_for_cursor(void *array0, int SIZE,  position *cursor) {
  int (*array)[SIZE] = array0;
  if (cursor->y == SIZE - 1) return false;
  return true;
}
// this function verifies if the left of the player is free
bool is_left_free_for_cursor(void *array0, int SIZE, position *cursor) {
  int (*array)[SIZE] = array0;
  if(cursor->y == 0) return false;
  return true;
}
// this function verifies if up is free
bool is_up_free_for_cursor(void *array0, int SIZE, position *cursor) {
  int (*array)[SIZE] =array0;
  if (cursor->x == 0) return false;
  return true;
}
// this function verifies if down is free
bool is_down_free_for_cursor(void *array0, int SIZE, position *cursor) {
  int (*array)[SIZE] = array0;
  if (cursor->x == SIZE - 1) return false;
  return true;
}
// this function verifies if a direction is free
bool is_direction_free_for_cursor(void *array0, int SIZE, position *cursor, char direction) {
  int (*array)[SIZE] = array0;
  if      (direction == 'w') is_up_free_for_cursor(array, SIZE, cursor);
  else if (direction == 'd') is_right_free_for_cursor(array, SIZE, cursor);
  else if (direction == 'a') is_left_free_for_cursor(array, SIZE, cursor);
  else if (direction == 's') is_down_free_for_cursor(array, SIZE, cursor);
}

// this function changes the cursor to a certain direction
void change_cursor_to_direction(position *cursor, char direction) {
  if      (direction == 'w') up(cursor);
  else if (direction == 'd') right(cursor);
  else if (direction == 's') down(cursor);
  else if (direction == 'a') left(cursor);
}

// this function checks if the direction is free and then it
// moves the cursor
void move_cursor_to_direction(void *array0, int SIZE, position *cursor, char direction) {
  int (*array)[SIZE] = array0;
  if (is_direction_free_for_cursor(array, SIZE, cursor, direction)) {
    change_cursor_to_direction(cursor, direction);
  }
}

// this function prints the whole matrix and the current position
void print_matrix_and_cursor(int size, int array[][size], position *cursor) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (i == cursor->x && j == cursor->y) {
	if (array[i][j] == 1) {
	  blue(); printf("@;");
	} 
	else if (array[i][j] == 2) {
	  blue(); printf("A;");
	}
	else if (array[i][j] == 0) {
	  blue(); printf("CT");
	}
	else if (array[i][j] == 3) {
	  blue(); printf("hp");
	}
	else if (array[i][j] == 4) {
	  blue(); printf("pp");
	}
	else if (array[i][j] == 7) {
	  blue(); printf("BB");
	}
      }
      else if (array[i][j] == 4) print_potenzializer();
      else if (array[i][j] == 3) print_health_potion();
      else if (array[i][j] == 2) print_trap();
      else if (array[i][j] == 1) print_wall();
      else if (array[i][j] == 7) print_boss();
      else print_empty();
    }
    printf("\n");
  }
}

// this function changes the number from the current position
// in the array
void mod_maze_position(void *array0, int SIZE, position *cursor, int num) {
  int (*array)[SIZE] = array0;
  if (array[cursor->x][cursor->y] == num) clear_position(array, SIZE, cursor);
  else array[cursor->x][cursor->y] = num;
}


// this function puts a zero in all of
// the maze positions
void zero_maze_and_one_borders(int N, int maze[][N]) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (i == 0 || j == 0 || i == N - 1 || j == N - 1) maze[i][j] = 1;
      else maze[i][j] = 0; 
    }
  }  
}
// this function prints as maze to a file in txt mode
void print_maze_to_file(int N, int maze[][N], char * str_name) {
  FILE *p_file = fopen(str_name, "w");
  if (p_file == NULL) {
    printf("Error, file not open\n");
    exit(1);
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      fprintf(p_file, "%d ", maze[i][j]);
    }
    fprintf(p_file, "\n");
  }
  fclose(p_file);
}


int jokenpo() {

  int jgr, maq;

  printf("\nEscolha:\n");
  printf("1 - pedra\n");
  printf("2 - papel\n");
  printf("3 - tesoura\n");

  scanf("%d", &jgr);


  maq = 1 + (rand() % 3);

  if (maq == 1) {
    printf("\nBig Boss escolheu pedra\n");
  } else if (maq == 2) {
    printf("\nBig Boss escolheu papel\n");
  } else if (maq == 3) {
    printf("\nBig Boss escolheu tesoura\n");
  }

  if (jgr == maq) {
    printf("\nEmpate\n");
    return 0;
  } else if (jgr == 1 && maq == 3 || jgr == 2 && maq == 1 || jgr == 3 && maq == 2) {
    printf("\nVocê ganhou a rodada\n");
    return 1;
  } else {
    printf("\nBig Boss ganhou a rodada\n");
    return -1;
  }

}


int melhor_de_3() {
  int total = 0, flag = 1, partidas = 0;
  srand( time(NULL) );

  while (flag) {
    total += jokenpo();
    partidas++;

    if (abs(total) == 2) {
      flag = 0;
    } else if (partidas == 3) {
      flag = 0;
    }
  }

  return total;

}


int resultado_partida() {
  
  int resultado;
  
  do
    {
      resultado = melhor_de_3();

      if (resultado > 0) {
	printf("\nVocê ganhou a melhor de três\n");
	tres_charadas(1, 1);
	return 1;
      } else if (resultado < 0) {
	printf("\nBig Boss ganhou a melhor de três\n");
	return 0;
      } else {
	printf("\nEmpate na melhor de três, você irá jogar novamente\n");
      }	
    } while (resultado == 0);

    
}


int questao_cc_1() {

  int choice;
  
  printf("\nQual das seguintes formas não é uma representação "
	 "do pequeno teorema de Fermat?\n"
         "Sendo p um número primo e a um número inteiro.\n");
  
  do
    {
      printf("\n1. p | a^p - a\n");
      printf("2. a^p ≅ a (mod p)\n");
      printf("3. a^(p-1) ≅ 2 (mod p)\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 3) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_cc_2() {

  int choice;


  printf("\nDentre as seguintes opções para o tempo de um algoritmo, "
	 "qual é a menos eficiente?\n");
  
  do
    {
      printf("\n1. linearitmico - O(n log n)\n");
      printf("2. linear - O(n)\n");
      printf("3. logaritmico - O(log n)\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_cc_3() {

  int choice;


  printf("\n32 times qualificaram para a copa do mundo de 2018, "
	 "se os nomes dos times fosse arranjandos em ordem "
	 "alfabética - numa array - quantos itens na array um "
	 "algoritmo de escolha binária teria que examinar "
	 "para encontrar um time em particular, no pior caso?\n");
  
  do
    {
      printf("\n1. No máximo, 32 times\n");
      printf("2. No máximo, 16 times\n");
      printf("3. No máximo, 6 times\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 3) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_cc_4() {

  int choice;


  printf("\nDentre as seguintes alternativas, qual não é um "
	 "princípio de POO (programação orientada a objetos)?\n ");
  
  do
    {
      printf("\n1. Encapsulamento\n");
      printf("2. Polimorfismo\n");
      printf("3. Identidade\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 3) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_cc_5() {

  int choice;


  printf("\nUm byte contém quantos bits?\n");
  
  do
    {
      printf("\n1. 8\n");
      printf("2. 2⁸\n");
      printf("3. 256\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_cc_6() {

  int choice;


  printf("\nO tipo de cálculo característico de linguagens funcionais é o:\n");
  
  do
    {
      printf("\n1. Lambda\n");
      printf("2. Beta\n");
      printf("3. Continuo\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}


int questao_dd_1() {

  int choice;


  printf("\nA individualização da pena será regulamentada por lei. \n"
	 "E dentro as penas admissíveis, não figura:\n");
  
  do
    {
      printf("\n1. privação ou restrição de liberdade\n");
      printf("2. de trabalhos forçados\n");
      printf("3. prestação social afirmativa\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_dd_2() {

  int choice;


  printf("\nAssinale dentre as afirmativas expostas, a sentença \n"
	 "verdadeira em relação dos direitos e deveres individuais \n"
	 "e coletivos:\n");
  
  do
    {
      printf("\n1. É livre a locomoção em tempo de paz, no território nacional, \n"
	     "permitindo a todos entrarem, saírem ou permanecerem nele com seus bens\n");
      printf("2. É plena a liberdade de associações para fins lícitos e paramilitares\n");
      printf("3. É garantido a todos o acesso à informação, sendo sempre sua fonte\n "
	     "pública a todo, uma vez que no art. 5° é vedado o anonimato\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_dd_3() {

  int choice;


  printf("\nNão é permitido, segundo o art. 5° da Constituição Federal: \n");
  
  do
    {
      printf("\n1. a dissolução ou suspensão compulsória de atividades da associação \n"
	     "por simples decisão judicial desde que transitada em julgado\n");
      printf("2. compelir alguém a associar-se ou a permanecer associado de algo\n");
      printf("3. a criação de associações independem de autorização, sendo vedada a \n "
	     "interferência estatal em seu funcionamento\n");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_dd_4() {

  int choice;


  printf("\nA propriedade intelectual garante: \n");
  
  do
    {
      printf("\n1. a propriedade industrial e os direitos do autor \n");
      printf("2. a criação particular e a industrial\n");
      printf("3. a posse de meios para a criação intelectual de projetos \n ");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_dd_5() {

  int choice;


  printf("\nA Constituição de 1988, reconhece a instituição do júri, porém não assegura: \n");
  
  do
    {
      printf("\n1. o sigilo de votações\n");
      printf("2. a competência para o julgamento dos crimes dolosos e culposos contra a vida\n");
      printf("3. a soberania dos veredictos \n ");

      scanf("%d", &choice);
      
    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_dd_6() {

  int choice;

  printf("\nA República Federativa do Brasil não tem como fundamento: \n");

  do
    {
      printf("\n1. a soberania\n");
      printf("2. o pluralismo político\n");
      printf("3. garantia de desenvolvimento nacional\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 3) {
    return 1;
  } else {
    return 0;
  }
  
}


int questao_med_1() {

  int choice;

  printf("\nCom relação a imunologia, julgue os itens subsequentes. \n");

  do
    {
      printf("\n1. O leite materno possui anticorpos que são tranferidos da mãe para o filho, \n"
	     "caracterizando a imunidade do tipo celular\n");
      printf("2. Suponha que uma criança tenha recebido uma vacina contra determinada doença \n"
	     "nos meses de janeiro, julho e dezembro do mesmo ano. Nesse caso, é correto \n"
	     "afirmar que a quantidade de anticorpos produzida pelo organismo é maior em dezembro \n"
	     "que em janeiro\n");
      printf("3. Os linfócitos A e D estão envolvidos na imunidade ativa\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }

}

int questao_med_2() {

  int choice;

  printf("\nDesde o surgimento da gripe suína, vacinas têm sido desenvolvidas na tentativa de \n"
	 "estabelecer um método de proteção para a população. Assinale a alternativa que \n"
	 "apresenta o mecanismo clássico de imunização em que se baseiam as vacinas. \n");

  do
    {
      printf("\n1. Imunização ativa - mecanismo, segundo o qual se introduz uma pequena quantidade de \n"
	     "antígenos no organismo para produção de anticorpos\n");
      printf("2. Imunização passiva - mecanismo, segundo o qual se introduz uma grande quantidade de \n"
	     "antígenos para a produção de anticorpos\n");
      printf("3. Imunização sativa - mecanismo, segundo o qual se introduz uma grade quantidade de \n"
	     "elementos no organismo para a produção de ideias\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 1) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_med_3() {

  int choice;

  printf("\nA vacina contra o H1N1, assim como qualquer outra vacina contra agentes causadores de doenças \n"
	 "infectocontagiosas, aumenta a imunidade porque: \n");

  do
    {
      printf("\n1. estimula a produção de glóbulos vermelhos pela medula óssea\n");
      printf("2. possui linfócitos B e T que neutralizam o agente causador da doença\n");
      printf("3. estimula a produção de anticorpos contra o agente causador da doença\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 3) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_med_4() {

  int choice;

  printf("\nNo caso de transplantes de orgãos, o processo de aceitação/rejeição do órgão transplantado \n"
	 "pelo indivíduo receptor está diretamente relacionado à(ao): \n");

  do
    {
      printf("\n1. sistema nervoso do receptor\n");
      printf("2. sistema imunológico do receptor\n");
      printf("3. sistema operacional linux\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_med_5() {

  int choice;

  printf("\nOs lipídios mais conhecidos são os glicerídeos, as ceras, os esteróides e os carotenoides. \n"
	 "Sobre os lipídios é correto afimar que: \n");

  do
    {
      printf("\n1. Eles são majoritariamente hidrofílicos\n");
      printf("2. O colesterol é um lipídio importante para o corpo humano porque ele compõe as membranas \n"
	     "celulares e é precursor de hormônios esteroides\n");
      printf("3. Os lipídios atuam como catalisadores biológicos\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}

int questao_med_6() {

  int choice;

  printf("\nAs moléculas mais utilizadas pela maioria das células para os processos de conversão de energia e \n"
	 "produção de ATP (trifosfato de adenosina) são os carboidratos. O armazenamento de lipídeos é vantajoso \n"
	 "sobre o de carboidratos pelo fato de terem a característica de serem: \n");

  do
    {
      printf("\n1. Sexy\n");
      printf("2. Majoritariamente hidrofóbicos\n");
      printf("3. Pouco biodegradáveis\n");

      scanf("%d", &choice);

    } while (escolha_invalida(choice));

  if (choice == 2) {
    return 1;
  } else {
    return 0;
  }
  
}


int num_aleatorio() {

  int resultado;

  srand( time(NULL) );
  resultado = 1 + (rand() % 6);

  return resultado;
}


int selecionador_de_questoes(int disciplina, int num) {

  if (disciplina == 1) {
    switch (num) {
    case 1: {
      return questao_cc_1();
    }
    case 2: {
      return questao_cc_2();
    }
    case 3: {
      return questao_cc_3();
    }
    case 4: {
      return questao_cc_4();
    }
    case 5: {
      return questao_cc_5();
    }
    case 6: {
      return questao_cc_6();
    }
    }
  } else if (disciplina == 2) {
    switch (num) {
    case 1: {
      return questao_dd_1();
    }
    case 2: {
      return questao_dd_2();
    }
    case 3: {
      return questao_dd_3();
    }
    case 4: {
      return questao_dd_4();
    }
    case 5: {
      return questao_dd_5();
    }
    case 6: {
      return questao_dd_6();
    }
    }
  } else if (disciplina == 3) {
    switch (num) {
    case 1: {
      return questao_med_1();
    }
    case 2: {
      return questao_med_2();
    }
    case 3: {
      return questao_med_3();
    }
    case 4: {
      return questao_med_4();
    }
    case 5: {
      return questao_med_5();
    }
    case 6: {
      return questao_med_6();
    }
    }
  }
}


int repetidor_de_questao(int disciplina, int num) {

  int var;

  var = selecionador_de_questoes(disciplina, num);

  if (var == 0) {
    printf("\nVocê errou a questão, tente novamente.\n");
    var = selecionador_de_questoes(disciplina, num);
  }

  return var;
  
}

int tres_charadas(int disciplina, int meta) {

  int num1, num2, num3, q1, q2, q3;

  
  num1 = num_aleatorio();

  printf("\nVocê poderá errar cada questão uma vez.\n"
	 "\nEm outras palavras, se você errar uma questão duas vezes "
	 "é... game over\n");

  do
    {
      num2 = num_aleatorio();
    } while (num2 == num1);

  do
    {
      num3 = num_aleatorio();
    } while (num3 == num2 || num3 == num1);

  q1 = repetidor_de_questao(disciplina, num1);

  if (q1 == 0) {
    printf("\nVocê errou a questão duas vezes.\n");
    return 0;
  }

  if (meta != 3) {
    q2 = repetidor_de_questao(disciplina, num2);

    if (q2 == 0) {
      printf("\nVocê errou a questão duas vezes.\n");
      return 0;
    }
  }


  if (meta == 1) {
    q3 = repetidor_de_questao(disciplina, num3);

    if (q3 == 0) {
      printf("\nVocê errou a questão duas vezes. E perdeu\n");
      return 0;
    }
  }


  return 1;
  
}

int play_maze(character_type test_player)
{
  int r_integer = random_integer(1, 5), status_turns = 0, total_turns = 0, item;
  int size = get_difficulty() * 30;
  int matrix[size][size];

  char maze_file_name[12];
  
  name_of_the_maze_file(size, r_integer, maze_file_name);
  
  get_array_from_file(maze_file_name, size, matrix);
  
  position player_position = {1, 2};
  change_position(&matrix, size, &player_position);
  printf("\n");
  print_close_matrix(size, matrix, &player_position, &test_player);
  char keypress;
  
 do {
   if (test_player.health_points == 0) return -1;
   keypress = getch();
   total_turns++;
   system("clear");
   printf("\n");
   move_player(matrix, size, &player_position, keypress, &test_player);
   item = matrix[player_position.x][player_position.y];
   change_player_stats_from_position(&test_player, item, &status_turns);
   change_position(matrix, size, &player_position);
   back_to_normal_stats(&test_player, &status_turns);
   
   print_close_matrix(size, matrix, &player_position, &test_player);
   print_player_stats(&test_player);
   printf("Status turns: %d\n", status_turns);
 } while (keypress != '0');
  
  return 0;
}

void create_maze()
{
  int N;
  char str_name[40], new_direction;
  position this_cursor = {0, 0};
  printf("What is the name of the file you want to create?\n");
  scanf(" %s", str_name);
  printf("What are the dimensions of the maze?\n");
  scanf("%d", &N);

  int maze[N][N];

  zero_maze_and_one_borders(N, maze);
  print_matrix_and_cursor(N, maze, &this_cursor);

  
  do
    {
      new_direction = getch();
      system("clear");
      
      if      (new_direction == 'c') mod_maze_position(maze, N, &this_cursor, 1);
      else if (new_direction == 't') mod_maze_position(maze, N, &this_cursor, 2);
      else if (new_direction == 'h') mod_maze_position(maze, N, &this_cursor, 3);
      else if (new_direction == 'p') mod_maze_position(maze, N, &this_cursor, 4);
      
      move_cursor_to_direction(maze, N, &this_cursor, new_direction);
      print_matrix_and_cursor(N, maze, &this_cursor);

      printf("c puts a wall\n");
      printf("t puts a trap\n");
      printf("h puts a heath potion\n");
      printf("p puts a potencializer\n");
      printf("0 exit\n");
      
    } while (new_direction != '0');

  print_maze_to_file(N, maze, str_name);
  
}


void update_maze()
{
  int N;
  char str_name[40], new_direction;
  position this_cursor = {0, 0};
  printf("What is the name of the file you want to update?\n");
  scanf(" %s", str_name);
  printf("What are the dimensions of the maze?\n");
  scanf("%d", &N);

  int maze[N][N];

  get_array_from_file(str_name, N, maze);
  print_matrix_and_cursor(N, maze, &this_cursor);

  
  do
    {
      new_direction = getch();
      system("clear");
      
      if      (new_direction == 'c') mod_maze_position(maze, N, &this_cursor, 1);
      else if (new_direction == 't') mod_maze_position(maze, N, &this_cursor, 2);
      else if (new_direction == 'h') mod_maze_position(maze, N, &this_cursor, 3);
      else if (new_direction == 'p') mod_maze_position(maze, N, &this_cursor, 4);
      else if (new_direction == 'b') mod_maze_position(maze, N, &this_cursor, 7);
       
      move_cursor_to_direction(maze, N, &this_cursor, new_direction);
      print_matrix_and_cursor(N, maze, &this_cursor);

      printf("c puts a wall\n");
      printf("t puts a trap\n");
      printf("h puts a heath potion\n");
      printf("p puts a potencializer\n");
      printf("0 exit\n");
      
    } while (new_direction != '0');

  print_maze_to_file(N, maze, str_name);
  
}
