/*
Name: Karan Parikh      NetID:kap9580       N Number:N12611540
Aim: To create a blood pressure monitoring system
Methodolgy: Calculation done using Slope (completely in C). Detailed method is stated before every function
Assumptions: Deflation rate is taken between 0.5 and 1.5 mm Hg because 4mm Hg drop is too large to catch any significant data
points. Sampling rate is at 0.5s
*/

//Header Files
#include "mbed.h"   
#include "USBSerial.h"
#include "stdio.h"

void calculate_Pressure();
void calculate_SystolicPressure();
void calculate_DiastolicPressure();
void calculate_HeartRate();


USBSerial serial; //creating USBSerial object

I2C pressure_Sensor(I2C_SDA, I2C_SCL); //creating I2C Object

Timer t; //Creating a Timer Object

//All pressure calculation variables
float pressure_Min = 0.0; //This is the lowest value in mm Hg that our pressure sensor can read
float pressure_Max = 300.0; //This is the highest value in mm hg that our pressure sensor can read
float pressure_Slope[1000] = {1.0}; //Array to store al the slopes between various pressure data points
float pressure = 0.0; //Stores the pressure calculated from the sensor
bool pressure_Increase = true, pressure_Decrease=false; //to check if pressure is increasing or decreasing
int pressure_Loop_Counter = 0; //Loop counter variable for pressure array , it stores the total values recorded from the sensor between 150 mm hg and 30 mm hg
float pressure_array[1000]; // Array to store all the pressure values
float previous_Pressure =0.0; //Stores the previous pressure value that was calculated
float max_PositiveSlope=0; //Stores the value of the maximum positive slope
int index_Of_MaxPositiveSlope = 0;//Stores the index of the maximum positive slope in the slope arrat
int systolic_Pressure_SlopeIndex = 0; //Stores the index of the Systolic Pressure in the pressure array
int diastolic_Pressure_SlopeIndex = 0;//Stores the index of the Diastolic Pressure in the pressure array
float heartrate_Count = 0; //Counts the number of positive slopes throughout the graph


//Deflation rate variables
char deflation_Rate_Fast[30] = "Deflation Rate too fast!"; //stores the comment "Deflation Rate too fast!"
char deflation_Rate_Slow[30] = "Deflation Rate too slow!";//stores the comment "Deflation Rate too slow!"
char deflation_Rate_Perfect[30] = "Deflation Rate is perfect!";//stores the comment "Deflation Rate is perfect!"
char *deflation_Rate_Final_Remark; //Used to store the final comment to display as per the condition

//All Sensor related variables
int sensor_Address = (0x18 << 1); //stores the address of the sensor to send during I2C communication
int sensor_Address_Read = 0x31; //stores the address to send for the read command of the sensor
const char sensor_Command[] = { 0xAA, 0x00, 0x00 }; //stores the commands to send in order for I2C write communication
char sensor_Result[4] = {0}; //Stores the 4 byte result given back by the sensor on reading from the sensor
float sensor_Output = 0.0; //Stores the output value calculated using "sensor_result"
float sensor_Output_Min = 419430.4; //Stores the minimum output given by the sensor as per the datasheet
float sensor_Output_Max = 3774873.6; //Stores the maximum output given by the sensor as per the datasheet
char sensor_Result_Status = '0'; //stores the value returned back on sending the read command to the sensor

float time_array[1000]; //creating a time array

//All Patient related variables
char patient_name[100]; //stores the name of the patient
int patient_age=0; //stores the age of the patient
char patient_gender[15]; //stores the gender of the patient
//float pressure_array[]={170.81, 170.52, 170.22, 169.92, 169.65, 169.42, 169.09, 168.84, 168.50, 168.28, 168.01, 167.73, 167.49, 167.17, 166.94, 166.70, 166.43, 166.15, 165.87, 165.60, 165.40, 165.09, 164.87, 164.70, 164.74, 164.70, 164.57, 164.32, 164.02, 163.58, 163.18, 162.85, 162.54, 162.20, 161.93, 161.63, 161.41, 161.15, 160.87, 160.61, 160.40, 160.16, 159.95, 159.76, 159.56, 159.26, 159.01, 158.74, 158.45, 158.23, 157.92, 157.67, 157.39, 157.16, 156.92, 156.65, 156.40, 156.17, 155.91, 155.69, 155.40, 155.21, 154.97, 154.72, 154.49, 154.26, 154.02, 153.77, 153.48, 153.31, 153.01, 152.78, 152.60, 152.52, 152.62, 152.69, 152.59, 152.44, 152.09, 151.66, 151.17, 150.81, 150.50, 150.17, 149.93, 149.69, 149.44, 149.18, 148.96, 148.72, 148.50, 148.36, 148.20, 147.99, 147.82, 147.55, 147.29, 147.07, 146.84, 146.53, 146.28, 146.07, 145.80, 145.62, 145.36, 145.13, 144.87, 144.67, 144.51, 144.22, 143.96, 143.75, 143.57, 143.37, 143.13, 142.97, 142.69, 142.53, 142.27, 142.03, 141.83, 141.58, 141.40, 141.17, 140.94, 140.75, 140.80, 140.96, 141.08, 141.09, 140.94, 140.74, 140.37, 139.98, 139.53, 139.07, 138.76, 138.50, 138.25, 138.00, 137.82, 137.58, 137.45, 137.16, 137.09, 136.97, 136.79, 136.57, 136.41, 136.23, 135.92, 135.71, 135.50, 135.25, 135.03, 134.78, 134.54, 134.36, 134.14, 133.94, 133.74, 133.50, 133.32, 133.12, 132.91, 132.76, 132.56, 132.36, 132.15, 131.94, 131.82, 131.56, 131.37, 131.18, 130.96, 130.69, 130.55, 130.38, 130.16, 130.03, 130.15, 130.38, 130.53, 130.54, 130.44, 130.20, 129.83, 129.42, 129.04, 128.59, 128.25, 127.92, 127.67, 127.42, 127.22, 126.98, 126.83, 126.70, 126.58, 126.47, 126.32, 126.17, 126.03, 125.83, 125.59, 125.35, 125.10, 124.88, 124.62, 124.43, 124.17, 123.96, 123.79, 123.61, 123.47, 123.23, 123.09, 122.91, 122.75, 122.57, 122.40, 122.23, 122.07, 121.85, 121.67, 121.43, 121.31, 121.11, 120.90, 120.71, 120.61, 120.79, 121.08, 121.30, 121.43, 121.36, 121.15, 120.81, 120.51, 120.18, 119.74, 119.38, 119.01, 118.70, 118.42, 118.17, 117.93, 117.74, 117.65, 117.48, 117.39, 117.26, 117.15, 117.01, 116.90, 116.70, 116.49, 116.30, 116.09, 115.84, 115.59, 115.39, 115.17, 115.02, 114.78, 114.64, 114.44, 114.34, 114.15, 113.97, 113.86, 113.69, 113.51, 113.40, 113.20, 113.08, 112.91, 112.74, 112.59, 112.42, 112.23, 112.11, 112.17, 112.51, 112.81, 113.07, 113.08, 112.95, 112.66, 112.38, 112.11, 111.82, 111.51, 111.22, 110.95, 110.66, 110.41, 110.13, 109.81, 109.58, 109.48, 109.34, 109.28, 109.17, 109.08, 108.95, 108.81, 108.57, 108.41, 108.26, 108.01, 107.82, 107.62, 107.39, 107.20, 107.03, 106.84, 106.68, 106.52, 106.33, 106.21, 106.02, 105.95, 105.77, 105.66, 105.58, 105.32, 105.20, 105.06, 104.95, 104.79, 104.59, 104.46, 104.38, 104.53, 104.82, 105.19, 105.48, 105.42, 105.31, 105.09, 104.86, 104.60, 104.40, 104.12, 103.91, 103.69, 103.46, 103.21, 102.96, 102.70, 102.46, 102.29, 102.16, 102.08, 102.01, 101.93, 101.82, 101.60, 101.45, 101.28, 101.07, 100.89, 100.66, 100.49, 100.26, 100.08, 99.89, 99.66, 99.49, 99.33, 99.17, 99.00, 98.85, 98.73, 98.62, 98.43, 98.34, 98.28, 98.10, 97.98, 97.81, 97.77, 97.54, 97.44, 97.33, 97.42, 97.74, 98.14, 98.48, 98.46, 98.34, 98.14, 97.94, 97.74, 97.54, 97.35, 97.12, 96.96, 96.73, 96.55, 96.30, 96.09, 95.89, 95.81, 95.76, 95.67, 95.60, 95.45, 95.37, 95.23, 95.04, 94.88, 94.71, 94.59, 94.38, 94.22, 94.04, 93.83, 93.66, 93.47, 93.27, 93.12, 92.98, 92.81, 92.65, 92.48, 92.39, 92.26, 92.18, 92.03, 91.88, 91.75, 91.56, 91.47, 91.39, 91.46, 91.77, 92.20, 92.35, 92.37, 92.25, 92.05, 91.90, 91.67, 91.47, 91.35, 91.12, 90.96, 90.76, 90.57, 90.38, 90.24, 90.12, 90.04, 89.94, 89.87, 89.83, 89.70, 89.54, 89.38, 89.25, 89.13, 88.94, 88.83, 88.63, 88.50, 88.32, 88.17, 87.99, 87.83, 87.69, 87.57, 87.44, 87.30, 87.15, 86.97, 86.91, 86.75, 86.62, 86.46, 86.32, 86.26, 86.27, 86.52, 86.87, 87.01, 86.97, 86.93, 86.72, 86.59, 86.49, 86.35, 86.23, 86.13, 85.92, 85.83, 85.69, 85.48, 85.30, 85.21, 85.11, 85.02, 84.92, 84.81, 84.72, 84.61, 84.54, 84.37, 84.26, 84.14, 83.99, 83.83, 83.70, 83.57, 83.40, 83.26, 83.13, 83.01, 82.85, 82.74, 82.58, 82.50, 82.31, 82.23, 82.11, 82.00, 81.90, 81.72, 81.62, 81.48, 81.41, 81.46, 81.75, 81.96, 81.91, 81.95, 81.85, 81.74, 81.66, 81.51, 81.37, 81.32, 81.19, 81.06, 80.95, 80.83, 80.66, 80.52, 80.44, 80.36, 80.28, 80.22, 80.08, 79.95, 79.90, 79.77, 79.74, 79.60, 79.44, 79.28, 79.18, 79.10, 78.96, 78.88, 78.72, 78.64, 78.54, 78.34, 78.23, 78.12, 77.98, 77.90, 77.79, 77.71, 77.58, 77.51, 77.39, 77.24, 77.18, 77.03, 76.93, 76.82, 76.72, 76.83, 77.08, 77.19, 77.15, 77.08, 77.01, 76.92, 76.86, 76.74, 76.71, 76.58, 76.49, 76.35, 76.34, 76.19, 76.06, 75.96, 75.87, 75.85, 75.76, 75.64, 75.58, 75.53, 75.38, 75.29, 75.21, 75.16, 75.04, 74.95, 74.82, 74.75, 74.64, 74.54, 74.44, 74.33, 74.23, 74.08, 74.03, 73.94, 73.82, 73.72, 73.62, 73.50, 73.47, 73.41, 73.27, 73.19, 73.08, 73.03, 72.93, 72.85, 72.69, 72.63, 72.63, 72.81, 72.98, 72.93, 72.91, 72.86, 72.79, 72.73, 72.63, 72.60, 72.52, 72.43, 72.35, 72.30, 72.20, 72.09, 71.97, 71.91, 71.90, 71.82, 71.81, 71.75, 71.65, 71.61, 71.52, 71.49, 71.39, 71.32, 71.27, 71.17, 71.08, 70.99, 70.94, 70.87, 70.78, 70.67, 70.61, 70.45, 70.35, 70.31, 70.19, 70.05, 70.01}; 

int main()
{
    t.start(); //timer start condition

    wait_ms(500); //wait for 500ms

    //print the program title 
    serial.printf("\n\n\n###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("***************** KP'S BLOOD PRESSURE MONITORING SYSTEM !!! *******************\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");
    serial.printf("###############################################################################\n");

    wait_ms(1000); //wait for 1000 ms

    serial.printf("\nENTER THE PATIENT'S NAME: "); //ask for patient's name
    serial.scanf("%s",&patient_name);
    serial.printf("\nENTER THE PATIENT'S AGE:");//ask for patient's age
    serial.scanf("%d",&patient_age);
    serial.printf("\nENTER THE PATIENT'S GENDER (Male/Female/Transgender): \n\r");//ask for patient's gender
    serial.scanf("%s",&patient_gender);

    serial.printf("\n****************************************");
    serial.printf("\n*********** Welcome %s ! ************",patient_name);
    serial.printf("\n****************************************\n");

    serial.printf("\nPLEASE NOTE: YOU WILL NEED TO PUMP THE CUFF BEYOND 150 MM HG FOR ACCURATE READINGS.\nDO NOT REMOVE THE CUFF AT ANY POINT TILL IT DROPS BELOW 30 MM HG\nDO NOT RELEASE THE PRESSURE TOO FAST, WAIT TILL THE INDICATORS SHOW ITS ACCURATE AND THEN KEEP THE VALVE STEADY");
    serial.printf("\n\nStart pumping the cuff begin the process and keep an eye out for the deflation rate indicator!\n\n");

    wait_ms(1000);

    calculate_Pressure(); //call the pressure calculation function that will print the MAP

    calculate_SystolicPressure(); //call the systolic pressure calculation function 

    calculate_DiastolicPressure(); //call the diastolic pressure calculation function 

    calculate_HeartRate(); //call the heart rate calculation function 
    
}


// ------------ Reading Pressure, Calculating Pressure Slope & MAP ------------
/*
Methodology:
1. Write the I2C write commands (0xAA, 0x00, 0x00) to the sensor at the address: 0x18 << 1 
This tells us the sensor to write at the following addresses. We read the status bit to check if its 1. 
If its 1, its busy so we wait till it clears.
Then we read the 4 byte output from the addresses 0xAA , 0x00 and 0x00.
2. Once we get the 4 bytes of the sensor data in "sensor_Result", we arrange them to create a 24 bit output 
(the first byte is the status byte and is not needed)
3.Using the formula given in the datasheet, we calculate the pressure reading
4.We check if the pressure is increasing or decreasing and we set values to pressure_Increase and pressure_Decrease accordingly
so that we only store values of our drop from 150 mm Hg to 30 mm Hg and ignore other values
5. As we start dropping from any high value above 150mm hg, we also check if the deflation rate is too fast/slow/perfect and print 
that along with the pressure reading. We do not store any pressure readings at this time
6. Once we reach 150mm hg, we only print and store the pressure readings and do not comment on the deflation rate because we will 
start getting pulses which will not give us a constant drop in pressure. We also store corresponding time values in the time_Array
7. Once we go below 30 mm Hg we stop calculating the pressure and break out of the loop to avoid calculating or storing
any values below 30mm Hg.
8.Once we have all the pressure readings from 150 mm Hg to 30 mm Hg, we will calculate the "pressure_Difference" for every
consecutive reading and the calculate the difference in time readings as "time_Difference".
9. Then we calculate the pressure_Slope using these two values : slope=(pressure_Difference / time_Difference). This basically
calculates the slope across the entire data considering every single pressure value and corresponding time value
10. When we get a pulse, we get a peak. A peak indicates that the slope will be positive as the graph won't constantly drop during
deflation but will peak a little(go upward a little) when blood flows before the graph starts falling again.
11. The Mean Arterial Pressure tends to have the highest peak (as per research articles)
12. To calculate the Mean Arterial Pressure, we find all the positive slopes in the slope array and find the 
maximum one out of those. Using this index from the slope array we print the same corresponding index in the pressure array
to get our Mean Arterial Pressure.

*/

void calculate_Pressure()
{

    while(1)
    {
        if(pressure > 150) //if pressure goes above 150 the first time(as we pump), then store pressure_Increase = false
            pressure_Increase = false; 


        if (!pressure_Increase && pressure < 151) //is pressure_Increase is false (i.e we finished pumping beyond 150) &&
        //now that we are dropping and pressure goes less than 151 mm Hg then make pressure_Decrease==true
            pressure_Decrease = true;

        if (pressure < 30 and pressure_Decrease) //if my pressure goes below 30 and pressure_Decrease is true that means
        //we were defalting when this happened then break out of the whole loop
             break;

        pressure_Sensor.write(sensor_Address, sensor_Command, 3); //Write the commands 0xAA, 0x00 and 0x00 at the given sensor address

        pressure_Sensor.read(sensor_Address_Read, &sensor_Result_Status, 1); //Read the status byte

        while(((sensor_Result_Status & 0x20) >> 5) == 0x1) //Reading the 5th bit in status byte and checking if its 1
        {
            pressure_Sensor.read(sensor_Address_Read, &sensor_Result_Status, 1);
            wait_ms(5);
        }

        pressure_Sensor.read(sensor_Address, sensor_Result, 4); //reading the sensor output and storing it in 4 bytes in snesor_Result array

        sensor_Output = (float)((sensor_Result[1] << 16) | (sensor_Result[2] << 8) | (sensor_Result[3])); //creating the 24-bit output from the sensor using bit shifting
        //the first byte is ignored as it is the status byte

        pressure = (((sensor_Output - sensor_Output_Min) * (pressure_Max - pressure_Min)) / (sensor_Output_Max - sensor_Output_Min)) + pressure_Min;
        //calculating pressure reading from the output as per the formula given in the datasheet.

        if((previous_Pressure-pressure)<0.5) //check if deflation rate is below 1.0
        {
            deflation_Rate_Final_Remark = deflation_Rate_Slow; // then print "Deflation rate is too slow!"
        }
        else if ((previous_Pressure-pressure)>1.5) //check if deflation rate is above 2.0
        {
            deflation_Rate_Final_Remark = deflation_Rate_Fast; //then print "Deflation rate is too fast!"
        }
        else //else if deflation rate is between 1.0 and 2.0
        {
            deflation_Rate_Final_Remark= deflation_Rate_Perfect; //then print "Deflation rate is perfect!"
        }
        int time_ms = t.read_ms(); //read the time value using ticker object "t"

        if(pressure_Decrease) //check if the pressure_Decrease==true , meaning that we are deflating and also going below 151mmHg
        {
            

            serial.printf (" Time: %d | Pressure Reading = %.2f \n", time_ms/1000, pressure); //only print pressure readings 
            pressure_array[pressure_Loop_Counter] = pressure; //store the current calculated pressure into the pressure array
            time_array[pressure_Loop_Counter] = time_ms/1000; //store the time in seconds in the time array
            pressure_Loop_Counter++; //increase the counter value by 1
            
        }
        else if(pressure_Increase == false) //if pressure is now pumped beyond 151 then start showing deflation rate remarks
        {
            serial.printf (" Time: %d | Pressure Reading = %.2f | %s \n", time_ms/1000, pressure, deflation_Rate_Final_Remark);
        }
        else
        {   //to print the increasing pressure when we pump the cuff
            serial.printf (" Time: %d | Pressure Reading = %.2f\n", time_ms/1000, pressure);
        }

        //storing the current pressure value into previous_Pressure before going to calculate the new pressure value
        previous_Pressure = pressure; 

        wait_ms(500); //sample at every 0.5 seconds
    }

    t.stop(); //stop the timer once all pressure calculations are over

    int loop1 = 0; //create loop variable
    int loop2 = 0; //create loop variable

    for(loop1=1;loop1<pressure_Loop_Counter;loop1++) //loop for calculating slopes
    {
        float pressure_Difference=pressure_array[loop1]-pressure_array[loop1-1]; //stores consecutive pressure differences
        float time_Difference=(time_array[loop1]-time_array[loop1-1]); //stores consecutive time differences

        if(time_Difference != 0.000000 ) //if time difference not zero (to prevent getting an infinite slope)
        pressure_Slope[loop1-1]= (pressure_Difference/time_Difference); //then store in slope array
        //serial.printf("Pressure diff, Time Diff, Max diff: %.2f %.2f %.2f\n\r", pressure_Difference, time_Difference, pressure_Slope[loop1-1]);
    }
    
    for(loop2=0; loop2<pressure_Loop_Counter; loop2++) //loop to find the maximum positive slope for MAP
    {

        if(pressure_Slope[loop2] > max_PositiveSlope)//check if current slope in the array greater than our current max positive slope
        {
            max_PositiveSlope = pressure_Slope[loop2];// then store the new greater value in max_PositiveSloep
            index_Of_MaxPositiveSlope = loop2+1; //store the index of this slope value, 
            //add 1 to it to use it in pressure array because we want the higher pressure reading used to calculate the postive slope
            
        }
    }

    serial.printf("\n######################################################################\n");
    serial.printf("*************************** RESULTS! *********************************\n");
    serial.printf("######################################################################\n\n");
    serial.printf("\nPATIENT NAME: %s\n",patient_name); //display patient name
    serial.printf("PATIENT AGE: %d\n",patient_age); //display patient age
    serial.printf("PATIENT's GENDER: %s\n",patient_gender); //display patient gender

    //print the Mean Arterial Pressure(MAP) after finding the corresponding index for the Maximum Positive Slope
    serial.printf("\nMEAN ARTERIAL PRESSURE: is %.2f\n", pressure_array[index_Of_MaxPositiveSlope]);
    
    
}



// ------------ Systolic Pressure Calculation -----------

/*
Methodology:
1. We know that Amplitude of the peak for the Systolic pressure reading is usually (0.5 X Amplitude of peak of MAP)
and lies above the MAP. Lets call (0.5 X Amplitude of peak of MAP) as sysSlopeMinThreshold (threshold value)
2. We already found amplitudes of all peak( these are the slopes! )
Using the formula above we calculate our systolic slope threshold.
3. Then we try to find the closest slope value (by calculating smallest difference between current slope reading and sysSlopeMinThreshold) 
to (0.5 X Amplitude of peak of MAP). 
4. The index of this reading will give us the slope of the Systolic Pressure. 
Using the slope index we can find the corresponding Systolic Pressure form the pressure array.
*/

void calculate_SystolicPressure()
{
    float sysSlopeMinThreshold = 0.5 * max_PositiveSlope; //calculate Systolic pressure slope minimum threshold
    
    float diffInSlope=0; //stores the difference in slope readings
    float minDiffInSlope=INT32_MAX; //stores the smallest difference in slope readings. Initially its a very large value.
   
    int loop3 = 0; //loop variable

    //run the loop through the slope array from the start till we reach a value lesser than the index of the slope of the MAP.
    for(loop3=0; loop3 < index_Of_MaxPositiveSlope-1; loop3++) 
     {
        //check first if slope is positive && is the slope less than our systolic threshold value
         if((pressure_Slope[loop3]>=0.0) && (pressure_Slope[loop3] < sysSlopeMinThreshold))
         {
            //calculate the difference between our threshold value and the current slope reading
            diffInSlope = sysSlopeMinThreshold - pressure_Slope[loop3]; 
            if(diffInSlope < minDiffInSlope) //check if the calculated difference is less than minimum difference in slope
            {
                minDiffInSlope=diffInSlope; //then store minimum difference in slope as this calculated difference
                systolic_Pressure_SlopeIndex=loop3+1;  //store the corresponding index for the value in pressure array
            }  
         }
     }
    //print the corresponding pressure reading from the pressure array as "Systolic Pressure"
    serial.printf("\nSYSTLOIC PRESSURE PATIENT: %.2f \n",  pressure_array[systolic_Pressure_SlopeIndex]);
    
   
}




// ------------ Diastolic Pressure Calculation -----------------

/*
Methodology:
1. We know that Amplitude of the peak for the Diastolic pressure reading is usually (0.8 X Amplitude of peak of MAP)
and lies below the MAP. Lets call (0.8 X Amplitude of peak of MAP) as diaSlopeMinThreshold (threshold value)
2. We already found amplitudes of all peak( these are the slopes! )
Using the formula above we calculate our Diastolic slope threshold.
3. Then we try to find the closest slope value (by calculating smallest difference between current slope reading and diaSlopeMinThreshold) 
to [0.8 X Amplitude of peak of MAP]. 
4. The index of this reading will give us the slope of the Diastolic Pressure. 
Using the slope index we can find the corresponding Diastolic Pressure form the pressure array.
*/
void calculate_DiastolicPressure()
{ 
    float diaSlopeMinThreshold = 0.8 * max_PositiveSlope; //calculate Diastolic pressure slope minimum threshold
    float diffInSlope_dia=0; //stores the difference in slope readings
    float minDiffInSlope_dia=INT32_MAX; //stores the smallest difference in slope readings. Initially its a very large value.

    int loop4 = 0; //loop variable

    //run the loop through the slope array from the index post MAP till we reach a value lesser than the last slope in the slope array
    for(loop4=index_Of_MaxPositiveSlope+1; loop4 < pressure_Loop_Counter; loop4++)
     {  
        //check first if slope is positive && is the slope less than our diastolic threshold value
        if((pressure_Slope[loop4]>=0.0) && (pressure_Slope[loop4] < diaSlopeMinThreshold))
        {
            //calculate the difference between our threshold value and the current slope reading
            diffInSlope_dia = diaSlopeMinThreshold - pressure_Slope[loop4];
            if(diffInSlope_dia < minDiffInSlope_dia) //check if the calculated difference is less than minimum difference in slope
            {
                minDiffInSlope_dia=diffInSlope_dia; //then store minimum difference in slope as this calculated difference
                diastolic_Pressure_SlopeIndex=loop4+1; //store the corresponding index for the value in pressure array
                
            }
        }
     }
    //print the corresponding pressure reading from the pressure array as "Diastolic Pressure"
    serial.printf("\nDIASTOLIC PRESSURE OF PATIENT: %.2f \n",  pressure_array[diastolic_Pressure_SlopeIndex]);
    
   
}
    


// ------------ Heart rate calculation ------------
/*
Methodology:
1. heartrate_Count will count all the positive slopes above threshold 0.0 between systolic pressure index 
   and diastolic pressure index. (The reason we use threshold is that we want to count only very significant pulses)
2. This is divided by the time difference corresponding to the systolic and diastolic indexes to give us the average heart beats
    per time frame
3. We then multiply this 60 to give us the heart beats per minute
*/
void calculate_HeartRate()
{
    
    int loop5=0; //initialize loop variable
    for(loop5 = systolic_Pressure_SlopeIndex-1; loop5 < diastolic_Pressure_SlopeIndex; loop5++) //run through the pressure slope loop
    {
        if(pressure_Slope[loop5] > 0.0)//check for all positive slopes greater than 0.0
            {
                heartrate_Count++; //count these positive slopes
            }    
    }


    int heart_Rate = (int)(((heartrate_Count) / (time_array[diastolic_Pressure_SlopeIndex] - time_array[systolic_Pressure_SlopeIndex]))* 60.0f);

    //calculate the heart rate by dividing by the time and then multiplying by 60
    serial.printf("\nHEART RATE OF PATIENT: %d beats per minute\n",heart_Rate);

    serial.printf("\n*************************** THANK YOU ! **************************************\n");
    
}
    




