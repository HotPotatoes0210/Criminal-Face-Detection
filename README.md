# Criminal Face Detection

This Project is made by TRAN DAI VIET HUNG - student from Sejong University, Seoul, South Korea.
* Please don't use this project for business or the other purpose except for education

Introduction: 
The Program is the criminial detection as it uses faces data which have been trained to recognize the facial expression of the criminal. It collects data by taking 10 potrait photos in 100x100 grayscale and store it in the local folder. The data will use the OpenCV library to training. 

How to use:
1. In the main.cpp please add your name_list.txt path and face_data folder path so the program can write the name to txt file and save and take the data from face data folder
2. Now run the code, before run the code. Please make sure that your name_list.txt file must be empty
3. Type the command
3.1: Please hold your face for 5 second to scanning the face
3.2: If the face data is empty, everyone is innocent. If the data is located then the criminal detection will work with the face of the people got scanned.
4. Program End, if you want to delete old face data please go to face data folder and delete data you want to remove and also delete the name of data in the name_list.txt
