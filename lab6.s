/* October 2019
 * 
 *
 */


#include"mbed.h" 
 
DigitalOut clk(p27);  
DigitalOut rst(p28);  
DigitalOut data(p30); 

DigitalIn row1(p26);
DigitalIn row2(p25);
DigitalIn row3(p24);
DigitalIn row4(p23);

DigitalOut col1(p8);
DigitalOut col2(p7);
DigitalOut col3(p6);
DigitalOut col4(p5);

//-----------Global Variables-----------//
int first=0;// first number entered
int temp1=0;//temporary variable used to store first number
int second=0;//second number entered
int temp2=0;//temporary variable used to store second number
int third=0;//Third number entered
int A=0;//first number for arithmetic operation
int B=0;//second number for arithmetic operation
int operand;//type of operation
int counter=0;//counter used to keep track the number of input(limited to two)
int arr[16][8] = {  {1,1,0,0,0,0,0,0}, //0 
				{1,1,1,1,1,0,0,1}, //1 
				{1,0,1,0,0,1,0,0}, //2 
				{1,0,1,1,0,0,0,0}, //3 
				{1,0,0,1,1,0,0,1}, //4 
				{1,0,0,1,0,0,1,0}, //5 
				{1,0,0,0,0,0,1,0}, //6 
				{1,1,1,1,1,0,0,0}, //7 
				{1,0,0,0,0,0,0,0}, //8 
				{1,0,0,1,1,0,0,0}, //9 
				{1,0,0,0,1,0,0,0}, //A
				{1,0,0,0,0,0,1,1}, //B
				{1,1,0,0,0,1,1,0}, //C
				{1,0,1,0,0,0,0,1}, //d
				{1,0,0,0,0,1,1,0}, //E
				{1,0,0,0,1,1,1,0}}; //F

// function to read the columns and rows from the keypad (4x4)
int readPad(int row1, int row2, int row3, int row4, int co1, int col2, int col3, int col4 ){

	// check row by row of the keypad matrix
	if (!row1){
		if(!col1)return 1;
		else if (!col2) return 2;
		else if (!col3) return 3;
		else if (!col4) return 10;
			}
	else if (!row2){
		if(!col1)return 4;
		else if (!col2) return 5;
		else if (!col3) return 6;
		else if (!col4) return 11;
	}
	else if (!row3){
		if(!col1)return 7;
		else if (!col2) return 8;
		else if (!col3) return 9;
		else if (!col4) return 12;
	}
	else if (!row4){
		if(!col1)return 0;
		else if (!col2) return 15;
		else if (!col3) return 14;
		else if (!col4) return 13;
	}
	else return 88;
}	//end readPad()
 
void EdgeClock(){ 
 wait(0.000001); 
 clk = 1; 
 wait(0.000001); 
 clk=0; 
  
} 

//------------------Displays the values on the 7-segment--------------//
void displayVal(int val1, int val2 , int val3){
	//print third digit MS
	for (int j = 0; j < 8 ; j++) { 	
		data = arr[val1][j]; 
		EdgeClock(); 
	} 	
	//print second digit
	for (int j = 0; j < 8 ; j++) { 	
		data = arr[val2][j]; 
		EdgeClock(); 
	}
	//print first digit LS
	for (int j = 0; j < 8 ; j++) { 	
		data = arr[val3][j]; 
		EdgeClock(); 
	}
	

	wait(0.2);// Paused to show 
}

//------------Clears the 7-segment display-----------//
void clearDisplay(){
	displayVal(0,0,0);
}

//-------------Perform the operations----------------//
void doMath(int operand, int a, int b){
	int result=0;//variable used to store the result
	clearDisplay();//clear the display
	if (operand==10){//Addition
		result=A+B;
	}else if(operand==11){//subtraction
		result=A-B;
	}else if(operand==12){//And
		result=A & B;
	}else if(operand==13){//Or
		result=A | B;
	}
	//Get each digit from result
	third=result/100;
	int temp=result%100;
	second=temp/10;
	first=temp%10;
	//Display the digits 
	displayVal(first,second, third);
}



//------------------Save the value of the key pressed-------------//
void saveValue(int val){
	// if number of digits is less than 2 and is a valid number 0-9, save the values for each digit
	if (counter<2 && val<10){
		temp1=first;
		first=val;
		temp2=second;
		second=temp1;
		third=temp2;
		counter++;
		displayVal(first,second, 0);
	//if number is 10(A), 11(B), 12(C), 13(D), then save the previous digits as A, and save the current value as operand
	}else if(val>9 && val != 14 && val != 15) {
		A=(10*second)+first;
		first=0;
		second=0;
		third=0;
		counter=0;
		displayVal(val,0,0);
		operand=val;
	//if number is F(15), then reset
	}else if (val==15){
		first=0;
		second=0;
		third=0;
		counter=0;
		displayVal(0,0,0);
	//if value is E(14), then save the previous digits as B, reset first, second, third and counter
  // and perform the correspondig operation
	}else{
		B=(10*second)+first;
		first=0;
		second=0;
		third=0;
		counter=0;
		displayVal(val,0,0);
		doMath(operand, first, second);
	}
}

void readKeyPad(){
int val;

		col1=0;
		col2=1;
		col3=1;
		col4=1;
		val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

		if (val!=88){saveValue(val);}
	
		col1=1;
		col2=0;
		col3=1;
		col4=1;
		val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);
		
		if (val!=88){saveValue(val);}
		
		col1=1;
		col2=1;
		col3=0;
		col4=1;
		val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

		if (val!=88){saveValue(val);}		
		
		col1=1;
		col2=1;
		col3=1;
		col4=0;
		val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

		if (val!=88){saveValue(val);}
}


int main(){ 
	rst = 0; 
	EdgeClock(); 
	rst = 1; 
	EdgeClock(); 
	displayVal(0,0,0);
	
	while(1){readKeyPad();}
}


