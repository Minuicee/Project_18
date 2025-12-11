#include <iostream>
#include <vector>
#include <numeric>
#include <bits/stdc++.h>

using namespace std;

//save 12x10 grid as __int128_t
__int128_t grid;

//declare functions
void print_b(__uint128_t num);
void clear_grid();
void print_grid(__uint128_t block);
void print_as_grid(string message,__uint128_t num);
void print_as_grid_compare(string message, __uint128_t num1, __uint128_t num2);
void init_blocks_free_space();
void init_col_masks();
void init();

//create a struct for direction types: up, down, right & left
//saves how many bits each square has to be shifted and which blocks_free_space direction matters
struct direction_type{ 
    int shift;
    u_int8_t free_space_mask_1;
    u_int8_t free_space_mask_2;
};

//define directions 
static direction_type UP = {-10, 1u, 1u << 1};
static direction_type DOWN = {10, 1u << 2, 1u << 3};
static direction_type LEFT = {-1, 1u << 4, 1u << 5};
static direction_type RIGHT = {1, 1u << 6, 1u << 7};

//global variables
static pair<int,int> grid_size = {12,10};

//init blocks in all 4 rotations
 u_int16_t blocks[7][4] = {
    {0x0F00, 0x2222, 0x0F00, 0x2222}, // I
    {0x0660, 0x0660, 0x0660, 0x0660}, // O
    {0x0E40, 0x4C40, 0x04E0, 0x4640}, // T
    {0x06C0, 0x8C40, 0x06C0, 0x8C40}, // S
    {0x0C60, 0x4C80, 0x0C60, 0x4C80}, // Z
    {0x08E0, 0x6440, 0x0E20, 0x44C0}, // J
    {0x02E0, 0x4460, 0x0E80, 0xC440}  // L
};

//variable for free space in every direction
//uses 8 bits, 2 bits per direction, in following order: up-down-left-right
u_int8_t blocks_free_space[7][4];
vector<__uint128_t> col_masks;
u_int16_t space_masks[4][2] = {
    {0xF000, 0xFF00}, // up1&up2
    {0x000F, 0x00FF}, // down1&down2
    {0x1111, 0x3333}, // left1&left2
    {0x8888, 0xcccc} // right1&right2
};
__int128_t block_move(__uint128_t block, int shift);
u_int16_t* get_block();
int get_msb(int num);
int get_lsb(int num);
int get_row(int num, int row_size);
int get_random_block_id();
vector<__uint128_t> create_blocks(int block_id);
int get_block_position(__uint128_t block);
//return possible moves with information in order: rotation, bits moved right, bits moved left
//every move of return vector is ordered as: rotation, up_movement, right_movement
vector<vector<int>> get_possible_moves(vector<__uint128_t> blocks, int block_id);

//variables for states
// int current_block_rotation;
// int current_block_position;


//main function
int main(){

    //init variables etc
    init();

    //test grid
    int block_id = 0;
    vector<__uint128_t> test_blocks = create_blocks(block_id);

    grid |= test_blocks[1];
    
    auto moves = get_possible_moves(test_blocks, block_id);
    cout << moves[0][0] << endl;
    cout << moves[0][1] << endl;
    cout << moves[0][2] << endl;
    
    print_grid((__uint128_t)0u);

    return 0;
}

__int128_t block_move(__uint128_t block, int shift){
    //shift left if shift_amount is positive, else right
    if(shift < 0){
        return block >> -shift;
    }
    else{
        return block << shift;
    }
}


vector<vector<int>> get_possible_moves(vector<__uint128_t> block_masks, int block_id){
    //init needed variable
    vector<vector<int>> possible_moves;
    pair<int, int> position;
    uint8_t free_space;
    int up_free_space;
    int down_free_space;
    int left_free_space;                    
    int right_free_space;
    int max_row_movement;
    int max_col_movement;
    int squares_to_move_up;
    __uint128_t block;
    __uint128_t col_mask;
    int cols_to_look_at;
    int blocks_lowest;
    int cols_highest;
    bool is_legal;
    
    if(grid == 0u){ // if the grid is empty return every move with up-movement = 0
        for(int r = 0; r < 4; r++){ 
            //get the free space from the block in every direction
            free_space = blocks_free_space[block_id][r];
            left_free_space = (free_space >> 4) & 3;
            right_free_space = (free_space >> 6) & 3;
            //calculate the max amount of moves that can be done left
            max_col_movement = grid_size.first-4+(right_free_space + left_free_space);
            for(int i = 0; i < max_col_movement; i++){
                possible_moves.push_back({r, 0, i});
            }
        }
        return possible_moves;
    }

    for(int rotation = 0; rotation < 4; rotation++){ //apply to every rotation
        //get the right block and reset position
        position.first = 0; //row
        position.second = 0; //col
        block = block_masks[rotation];
        free_space = blocks_free_space[block_id][rotation];

        //read the free space in every direction from the precomputed free_space integers
        up_free_space = free_space & 3;
        down_free_space = (free_space >> 2) & 3;
        left_free_space = (free_space >> 4) & 3;
        right_free_space = (free_space >> 6) & 3;

        //go up until either out of grid or guaranteed legal
        max_row_movement = grid_size.second-4+(up_free_space + down_free_space); //size of rows minus size of square plus non filled rows
        max_col_movement = grid_size.first-4+(right_free_space + left_free_space);  //size of cols minus size of square plus non filled cols
        blocks_lowest = 4-get_row(blocks[block_id][rotation], 4) - down_free_space; //lowest square

        //col mask for cols the block is in
        cols_to_look_at = 4-left_free_space-right_free_space;
        col_mask = accumulate(&col_masks[0],
            &col_masks[cols_to_look_at-1], //-1 because we start counting at 0 so we look at 0, 1, ..., cols_to_look_at-1
            (__uint128_t)0u, 
            [](__uint128_t a, __uint128_t b){ return a | b; });

        for(int col = 0; col <= max_col_movement; col++){
            //for new column move right
            if(col != 0){
                block = block_move(block, RIGHT.shift);
                col_mask <<= 1;
                position.second++;
            }

            //if there is nothing in the cols where the block is, just place it at the bottom
            if((grid & col_mask) == 0u){
                possible_moves.push_back({rotation, 0, col});
                continue;

            }
            print_as_grid_compare("grid, col_mask", grid , col_mask);
            //get highest square of current cols that the block is in
            cols_highest = get_row(grid & col_mask, grid_size.second);  //!cols highest might be wrong
            //move up to first place 1 below first guaranteed legal 
            squares_to_move_up = cols_highest-blocks_lowest; //12 - cols_highest - blocks_lowest + 1(because we want to be above that block)
            print_as_grid("squares_to_move_up", squares_to_move_up);
            //only do it if that wouldn't be above the grid
            if(squares_to_move_up < max_row_movement){
                //if there is a need to go up then do it
                if(squares_to_move_up > 0){
                    //but before moving block up move it down to position.first == 0 (it was combined so there only has to be one move operation instead of two)
                    //for example if the block is on 3 and has to go to 5, it will move up by (5-3 -1(because we want it to be the first place where collision can happen)) = 2
                    block = block_move(block, (squares_to_move_up*UP.shift)+(DOWN.shift*position.first+1));
                    position.first = squares_to_move_up-1; //move down then squares_to_move_up up
                } else {
                    block = block_move(block, DOWN.shift*(position.first)); //! ?test col masks
                    position.first = 0;
                }
            }
            else{
                continue;
            }

            is_legal = true;

            while(is_legal){ //move up until move is legal and then save it. if there is no legal move continue
                //test for any collision (no collision is legal move)
                is_legal = (grid & block) == 0u;
                if(!is_legal){ 
                    possible_moves.push_back({rotation, position.first+1, position.second});
                    break;
                }
                if(position.first < max_row_movement){
                    block = block_move(block, DOWN.shift);
                    position.first--;

                } else {
                    break;
                }       
            }
        }

    }
    return possible_moves;
}
int get_row(int num, int row_size){
    //only 1 bit of num can be 1
    if(num < 1) return 0;
    cout << __builtin_ctz(num);
    return __builtin_ctz(num)/row_size;
}

void print_as_grid_compare(string message, __uint128_t num1, __uint128_t num2){
    cout << endl;
    cout << message << endl;
    int index;
    __uint128_t element;
    __uint128_t block_mask;
    for(int row = 0; row < grid_size.first; row++){
        for(int col = 0; col < grid_size.second * 2; col++){
            if(col%grid_size.second==0) cout << "   ";
            if(col/grid_size.second){
                index = (col%grid_size.second)+row*grid_size.second;
                element = (num2 >> index) & 1u;
                if(element){ 
                    cout << "■";
                }else{
                    cout << "◻";
                }
            }
            else{
                index = col+row*grid_size.second;
                element = (num1 >> index) & 1u;
                if(element){ 
                    cout << "■";
                }else{
                    cout << "◻";
                }
            }
        }
        cout << endl;
    }
}

void print_as_grid(string message, __uint128_t num){
    cout << endl;
    cout << message << endl;
    int index;
    __uint128_t element;
    __uint128_t block_mask;
    for(int row = 0; row < grid_size.first; row++){
        for(int col = 0; col < grid_size.second; col++){
            index = col+row*grid_size.second;
            element = (num >> index) & 1u;
            if(element){ 
                cout << "■";
            }else{ 
                cout << "◻";
            }
        }
        cout << endl;
    }
}

void init_col_masks(){
    //init bit masks for cols in grid
    __uint128_t col_mask;

    for(int i = 0; i < grid_size.second; i++){ //for every col
        col_mask = 0u;
        for(int j = 0; j < grid_size.first; j++){ //for every row in that col
            col_mask |= (__uint128_t)1 << (j*grid_size.second + i);
        }
        col_masks.push_back(col_mask);
    }
}

void clear_grid(){
    //set all bits to zero
    grid = 0u;
}

void init(){
    //init necessary things

    //set random seet for rand() function
    srand(time(0));

    //init grid
    clear_grid();

    //init blocks_free_space
    init_blocks_free_space();

    //init bit masks for cols on the grid
    init_col_masks();

}

void init_blocks_free_space(){
    //order: up, down, left, right
    for(int block_id = 0; block_id < 7; block_id++){
        for(int rotation = 0; rotation < 4; rotation++){
            u_int8_t free_space = 0u;
            for(int direction = 0; direction < 4; direction++){
                for(int direction_amount = 0; direction_amount < 2; direction_amount++){
                    //loop through every block through every space mask
                    if((blocks[block_id][rotation] & space_masks[direction][1-direction_amount]) == 0u){ //start with bigger direction_amount. if its one, ignore the smaller one
                        free_space |= 1u << (direction*2+(1-direction_amount));
                        break;
                    } 
                }
            }
            blocks_free_space[block_id][rotation] = free_space;
        }
    }
}

void print_grid(__uint128_t block){
    //loop through bits to print them
    cout << endl;
    int index;
    __uint128_t element;
    __uint128_t block_mask;
    for(int row = 0; row < grid_size.first; row++){
        for(int col = 0; col < grid_size.second; col++){
            index = col+row*grid_size.second;
            element = (grid >> index) & 1u;
            block_mask = (block >> index) & 1u;
            if(element){ //if there is already something in grid print full square
                cout << "■";
            }
            else if(block_mask){ //if there is a block that is yet to be placed print a square with an asterisk
                cout << "⧆";
            }else{ //if there is nothing print an empty square
                cout << "◻";
            }
        }
        cout << endl;
    }
}

u_int16_t* get_block(){ //function is not used
    //get a random block
    //generate random number
    int r = rand()%7;
    return blocks[r];
}

int get_random_block_id(){
    return rand()%7;
}

vector<__uint128_t> create_blocks(int block_id){
    //return one block in all rotations as a 128bit mask
    vector<__uint128_t> masks;
    uint16_t block;
    int move_down_bits;
    int move_left_bits;

    //calculate where to position it
    for(int rotation = 0; rotation < 4; rotation++){
        __uint128_t mask = 0u;
        block = blocks[block_id][rotation];
        move_down_bits = ((blocks_free_space[block_id][rotation] >> 2) & 3) * grid_size.second; 
        move_left_bits = ((blocks_free_space[block_id][rotation] >> 4) & 3);
        for(int row = 0; row < 4; row++){
            u_int16_t row_bits = (block >> (row*4)+move_left_bits) & 0xF; 
            mask |= ((__uint128_t)row_bits << (((grid_size.first-1-row)*grid_size.second)+move_down_bits)); 
        }
        masks.push_back(mask);
    }

    return masks;
}
int get_msb(int num){
    if(num < 1) return 0;
    return 1<<(31 - __builtin_clz(num));
}

int get_lsb(int num){
    if(num < 1 ) return 0;
    return 1 << __builtin_ctz(num);
}

int get_block_position(__uint128_t block){
    //if there was a emtpy int return -1 as a sign of no position
    if(!block) return -1;

    //split the map in 2 64bit ints
    uint64_t low = (uint64_t)block;
    uint64_t high = (uint64_t)(block >> 64);

    if(!low){
        return __builtin_ctz(high)+64;
    }
    return __builtin_ctz(low);
}

void print_b(__uint128_t num){
    //print binary number
    cout << endl;
    vector<int> digits;
    for(int i = 0; i < 120; i++){
        digits.push_back((int)((num >> i) & 1u));
    }
    for(int i = 0; i < 120; i++){
        cout << digits.back();
        digits.pop_back();
    }
}