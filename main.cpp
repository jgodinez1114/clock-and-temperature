/* November 11, 2019
 * Lab 10
 *
 */


#include"mbed.h" 
 
 //BCD Values
 
#define decode_bcd(x)	((x >> 4) * 10 + (x & 0x0F))
#define encode_bcd(x)	((((x/10) & 0x0F) << 4) + (x % 10)) 
 
//temperature sensor
DigitalOut clk(p27);  
DigitalOut rst(p28);  
DigitalOut data(p30); 

//
DigitalIn row1(p26);
DigitalIn row2(p25);
DigitalIn row3(p24);
DigitalIn row4(p23); 

//clock module
//DigitalIn 

I2C i2c(p32,p31); //SDA and SCL pins

DigitalOut col1(p8);
DigitalOut col2(p7);
DigitalOut col3(p6);
DigitalOut col4(p5);

//-----------Global Variables-----------//

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


 
void EdgeClock(){ 
 wait(0.000001); 
 clk = 1; 
 wait(0.000001); 
 clk=0; 
  
} 

//function to convert from ASCII to value
int asciiToValue(int asciiVal){
	return decode_bcd(asciiVal);
}


//function to convert from value to ASCII
int valueToAscii(int val){
	return encode_bcd(val);
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

//function to parse the sensor reading to display
//work with unsigned reading
void parseMyReading(unsigned int temp){
	
	int dig1, dig2, dig3, dig4;
	float ctemp;
	ctemp = temp * 0.0625;
	
	dig1 = int(temp)%10;
	dig2 = temp/10;
	
	//display Celsius reading
	displayVal(dig1,dig2,12);
	
	float tempf;
	
	tempf = (temp * (1.8)) + 32;
	
	dig3 = int(tempf)%10;
	dig4 = tempf/10;
	
	wait(1.5);
	
	//display  Farenheit reading
	displayVal(dig3,dig4,15);
	
	wait(1.5);
}

//function to parse our time
//int sec, min, hour, day, date, month, year;
void parseMyTime(int sec, int min, int hour, int day, int date, int month, int year){
	int minuteDig1, minuteDig2, hrDig1, hrDig2, dateDig1, dateDig2, monthDig1, monthDig2, yrDig1, yrDig2;
	
	minuteDig1 = min % 10;
	minuteDig2 = min / 10;
	
	hrDig1 = hour % 10;
	hrDig2 = hour / 10;
	
	
	displayVal(minuteDig1, minuteDig2, hrDig1);	
	
	dateDig1 = date % 10;
	dateDig2 = date / 10;
	
	monthDig1 = month % 10;
	monthDig2 = month / 10;
	
	yrDig1 = year % 10;
	yrDig2 = year / 10;
	
	wait(3.2);
	
	displayVal(dateDig1, dateDig2, 0);
	wait(1.5);
	displayVal(monthDig1, monthDig2, 0);
	wait(1.5);
	displayVal(yrDig1, yrDig2, 13);
	wait(3.31);
	
	
	
}

//------------Clears the 7-segment display-----------//
void clearDisplay(){
	displayVal(0,0,0);
}

int main(){ 
	rst = 0; 
	EdgeClock(); 
	rst = 1; 
	EdgeClock(); 
	displayVal(0,0,0);
	
	
	int sec, min, hour, day, date, month, year;
	
	i2c.start();
	
	i2c.write(0xD0);
	
	i2c.write(0x00);
	
	i2c.write(0x00);
	
	i2c.stop();
	
  i2c.start();
	
	i2c.write(0xD0);
	
	i2c.write(0x00);
	
	i2c.write(0x00);	//we want 0
	
	i2c.write(0x00); //we want 0 minutes
		
	i2c.write(0x62); //we want 2 hours
			
	i2c.write(0x01); //we want day 1
				
	i2c.write(0x12); //we want date 11
					
	i2c.write(0x11);	//we want month 11
						
	i2c.write(0x19);	//we want year 19
	
	i2c.start();
	
	i2c.write(0xD0);
	
	i2c.write(0x0E);
	
	i2c.write(0x20);
	
	i2c.write(0);
	
	i2c.stop();
	
	//read
	
		
	unsigned int temp;	
	
	//start i2c communication using object
	i2c.start();	
	
	//arbitrary address for communications
	i2c.write(0x90);
	
	//access the 1-byte configuration register
	i2c.write(0xAC);
	
	//configure itself to continuously monitor temperature
	i2c.write(0x2);
	
	//communication from master is over. BYE
	i2c.stop();
	
	//collect data continuously
	i2c.start();
	
	i2c.write(0x90);
	
	//initiate temperature conversions
	i2c.write(0x51);
	
	i2c.stop();
	
	i2c.start();
	
	//writing to tell it to read 
	i2c.write(0x90);
	
	i2c.write(0xAA);	
	
	//continuously read from sensors
	while(1){
	
	// read from clock
	i2c.start();
	
	i2c.write(0xD0);
	
	i2c.write(0x00);
	
	i2c.start();
	
	i2c.write(0xD1);	
	
	sec   = i2c.read(1);	//ACK
	min   = i2c.read(1);
	hour  = i2c.read(1);
	day   = i2c.read(1);
	date  = i2c.read(1);
	month = i2c.read(1);
	year  = i2c.read(0);	//NACK
	
	//convert BCD to decimal 
	sec   = decode_bcd(sec);
	min   = decode_bcd(min);
	hour  = decode_bcd(hour);
	day   = decode_bcd(day);
	date  = decode_bcd(date);
	month = decode_bcd(month);
	year  = decode_bcd(year);
	
	parseMyTime(sec, min, hour, day, date, month, year);
	
	
	i2c.stop();
		
		//start communication with temp module
		i2c.start();
	
		//take reading from temperature module
		i2c.write(0x91);
		
		//get the first input
		temp = i2c.read(0);
		
		//end communications
		i2c.stop();
		
		//analyze reading from temperature sensor
		parseMyReading(temp);		

			
	}	//end while(1)
}	//end main()


