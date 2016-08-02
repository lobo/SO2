#include <game.h>

#define MIN_X 58
#define MAX_X 962
#define MIN_Y 180
#define MAX_Y 686

#define BLOCK_SIZE 20

#define MAX_LENGTH 10

#define HEADING_NORTH 0
#define HEADING_EAST 1
#define HEADING_SOUTH 2
#define HEADING_WEST 3

#define SNAKE_SPEED 20

#define MAX_NOTES_IN_SONG 64
#define NOTE_DATA_FIELDS 4

typedef struct _t_position{
	int x;
	int y;
} t_position;

typedef struct _t_snake{
	t_position * blocks[MAX_LENGTH];
	int heading;
	int length;
} t_snake;

unsigned short lfsr = 0xACE1u;
unsigned int bit;

int songBuffer[MAX_NOTES_IN_SONG][NOTE_DATA_FIELDS];
int cantNotes;

//thread variables
int snake_heading;
t_snake * snake;
t_position * food;

int rand(){
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

void end_game(){

	free(food);
	for(int i = 0 ; i < snake->length; i ++){
		free(snake->blocks[i]);
	}
	free(snake);

}

void new_food_position(t_position * f) {

	int xd =rand() % MAX_X;
	int yd = rand() % MAX_Y;

	if (xd < MIN_X) xd = MIN_X + 6;
	if (yd < MIN_Y) yd = MIN_Y + 6;

	f->x = xd;
	f->y = yd;

}

void add_block(int x, int y){

	snake->blocks[snake->length] = (t_position *) malloc(sizeof(t_position));
	snake->blocks[snake->length]->x = x;
	snake->blocks[snake->length]->y = y;

	snake->length+=1;
}

void game_print_string(char * str, unsigned int x, unsigned int y, int size){

	 for (int i = 0; i < strlen(str); i++){
	 	draw_char(str[i], x + i * size * 10, y, size, 255, 255, 255);
	 }
}

void draw_background(){
	game_print_string("SNAKE", 250, 60, 10);
	draw_rect(MIN_X, MIN_Y, MAX_X - MIN_X, MAX_Y - MIN_Y, 255, 255, 255);
}

void draw_food(){
	draw_circle(food->x, food->y, 5, 255, 0 , 0);
}

void new_game(){

	clear_screen();
	
	draw_background();

	//ESTADO INICIAL DE SNAKE Y FOOD
	draw_frect(MIN_X + 2, MIN_Y + 2, BLOCK_SIZE, BLOCK_SIZE, 100, 200, 0);
	add_block(MIN_X + 2,MIN_Y + 2);
	//add_block(s, 0, MIN_Y + (MAX_Y / 2));

	draw_food();

	flush_buffer();

}

int get_new_x(t_position old[], int pos){

	if (pos == 0){

		switch (snake_heading){

			case HEADING_EAST:
				return snake->blocks[0]->x + SNAKE_SPEED + 1;

			case HEADING_WEST:
				return snake->blocks[0]->x - SNAKE_SPEED - 1;

			default:
				return snake->blocks[0]->x;
		}

	}

	return old[pos -1].x;

}

int get_new_y(t_position old[], int pos){

	if (pos == 0){

		switch (snake_heading){

			case HEADING_NORTH:
				return snake->blocks[0]->y - SNAKE_SPEED - 1;

			case HEADING_SOUTH:
				return snake->blocks[0]->y + SNAKE_SPEED + 1;

			default:
				return snake->blocks[0]->y;
		}

	}

	return old[pos -1].y;

}

void move_snake(){

	int new_x, new_y;
	t_position old_pos[snake->length];

	for (int i = 0; i < snake->length; i ++){

		old_pos[i].x = snake->blocks[i]->x;
		old_pos[i].y = snake->blocks[i]->y;
	}

	for (int i = 0; i < snake->length; i ++){

		new_x = get_new_x(old_pos, i);
		new_y = get_new_y(old_pos, i);

		snake->blocks[i]->x = new_x;
		snake->blocks[i]->y = new_y;

		draw_frect(new_x, new_y, BLOCK_SIZE, BLOCK_SIZE, 100, 200, 0);
		
	}

}

void update_screen(){

	move_snake();
	//timers y eso

}

void render_screen(){

	draw_background();

	draw_food();

	flush_buffer();
}

//si hay colision, devuelve 1
//si gano, devuelve -1
//caso contrario, devuelve 0
int check_state(){

	t_position * head = snake->blocks[0];

	int head_edge_x_r = head->x + BLOCK_SIZE;
	int head_edge_x_l = head->x;
	int head_edge_y_t = head->y;
	int head_edge_y_b = head->y + BLOCK_SIZE;

	if (((head_edge_x_r > MAX_X) || (head_edge_x_l < MIN_X)) ){
		return 1; //engine run = false
	}

	if (((head_edge_y_t < MIN_Y) || (head_edge_y_b > MAX_Y)) ){
		return 1; //engine run = false
	}

	/*
	//si legth es mayor a 7?
	for (int i = 1; i < s->length; i++){
		int px = s->blocks[i]->x;
		int py = s->blocks[i]->y;

		if (snake_heading == HEADING_NORTH){
			if (head_edge_y_t < (py + BLOCK_SIZE)){
				beep();
				while (1);
				return 1;
			}
		}

		if (snake_heading == HEADING_SOUTH){
			if (head_edge_y_b > (py)){
				beep();
				while (1);
				return 1;
			}
		}

		if (snake_heading == HEADING_EAST){
			if (head_edge_x_r > (px)){
				beep();
				while (1);
				return 1;
			}
		}

		if (snake_heading == HEADING_WEST){
			if (head_edge_x_l < (px + BLOCK_SIZE)){
				beep();
				while (1);
				return 1;
			}
		}
	}
	*/

	//centro contenido, sin choque absoluto de bordes
	for (int j = -5; j < 5; j ++){
		if (head_edge_x_r > (food->x + j) && head_edge_y_b > (food->y + j) && head_edge_x_l < (food->x + j) && head_edge_y_t < (food->y + j)){
			add_block(0, 0);
			new_food_position(food);

			if (snake->length == (MAX_LENGTH - 1)){
				return -1;
			}
		}
	}

	return 0;

}

void loadSong(char* songFmt){

	//Formato de lectura: cantidad de notas-nota1-nota2. Cada nota se compone por a,b,c,d- con a=nota, b=duracion, c=octava y d=separacion con proxima nota
	char* songAux = songFmt;

	while (*songFmt != '-') //encapsular
		songFmt++;

	*songFmt = 0;

	cantNotes = atoi(songAux);

	songFmt++;
	songAux = songFmt;

	 for (int i = 0; i < cantNotes; i++){

		for (int j = 0; j < 4; j++){

			while ( (*songFmt != ',') && (*songFmt != '-') && (*songFmt != '\0') ) //fix
				songFmt++;
			*songFmt = 0;

			songBuffer[i][j] = atoi(songAux);

			songFmt++;
			songAux = songFmt;	
		}
	}
}

void game_audio(){

	char song_file[] = "18-7,6,0,7-7,6,0,7-7,6,0,7-3,6,0,7-10,3,0,2-7,3,0,8-3,6,0,7-10,3,0,2-7,9,0,10-2,6,1,7-2,6,1,7-2,6,1,7-3,6,1,7-10,5,0,4-6,5,0,8-3,6,0,7-10,3,0,2-7,6,0,7";
	loadSong(song_file);

	while (1) {

		for (int i = 0; i < cantNotes; i++){
			sys_sleep(get_pid(), songBuffer[i][3]);
			play_note(songBuffer[i][0],songBuffer[i][1],songBuffer[i][2]);
			sys_sleep(get_pid(), songBuffer[i][1]);
			mute();
		}
	}
}

void game_video(){

	int res;

	while (1){
		
		clear_screen();

		update_screen(); //podria no pasarlo porque son vars globales

		render_screen(); 

		res = check_state();

		if (res == 1){
			game_print_string("PERDISTE", 250, 400, 6);
			flush_buffer();
			while (1);
		}else if (res == -1){
			game_print_string("GANASTE!", 250, 400, 6);
			flush_buffer();
			while (1);
		}

	}

}

void game_entry_point(){

	snake = (t_snake *) malloc(sizeof(t_snake));
	snake->length = 0;

	food = (t_position *) malloc(sizeof(t_position));
	new_food_position(food);

	new_game();
	snake_heading = HEADING_EAST;

	int pid_audio = create_process("Game audio",&game_audio, FALSE); //play song
	int pid_video = create_process("Game video",&game_video, TRUE); //deberia tener un flag de THREAD para el listado

	while (1){
		
		char input = getchar();

		switch (input){

			case 'w':
				if (snake_heading != HEADING_SOUTH) snake_heading = HEADING_NORTH;
				break;

			case 'd':
				if (snake_heading != HEADING_WEST) snake_heading = HEADING_EAST;
				break;

			case 's':
				if (snake_heading != HEADING_NORTH) snake_heading = HEADING_SOUTH;
				break;

			case 'a':
				if (snake_heading != HEADING_EAST) snake_heading = HEADING_WEST;
				break;

			case 'q':
				end_game();
				kill(pid_audio);
				mute();
				kill(pid_video);
				kill(get_pid());
				break;

		}


	}


}