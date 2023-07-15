import tkinter as tk
from tkinter.filedialog import askopenfilename
import math
import serial
import time

#TODO: fix weird vibrations when the gcode finishes
#TODO: add a lookup feature (you see the
# - estimated finish time
# - where the cursor is
# - the travel lines are in different collors if the pen is going to draw over them

#serial comunications init
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.reset_input_buffer()

progresBarMax = 0;
progresBar = 0;

screenSizeX = 740#mm
screenSizeY = 550#mm
#position X, position Y, position Z (1-not drawing, 0-drawing)
positions = [[740/2,510,1]]
mouseX = 0
mouseY = 0

#TODO put all of the machine dimensions as a variable that i can setup
#where is the pencil at
posX = 740/2
posY = 510
posZ = 1
mode = "NORMAL"

filePath = "drawing.gcode"

#tkinter init
root = tk.Tk()
root.title("robotic arm")
root.geometry("740x510")
root.resizable(True,True)
Bplace=tk.Canvas(root,width=740,height=40)
Bplace.pack()
place=tk.Canvas(root,width=10,height=10)
place.pack(fill="both",expand=True)

def openFile():
	global posX
	global posY
	global posZ
	global positions
	global progresBarMax
	global filePath

	filePath = askopenfilename()

	#clear variables
	progresBarMax = 0
	clearDrawing()

	#read from gcode file
	try:
		file = open(filePath,"r")
		#reads the file to get a visualisation on the screen
		for line in file:
			for word in line.split():
				if word[0] == "(" or word[0] == ";":
					break#skips this loop if there is a comment

				if word == "G1" or word == "G0":
					pass

				if word[0] == "X":
					cleared_string = word.replace("X", "")
					try:
						posX = float(cleared_string)
					except ValueError:
						pass
				if word[0] == "Y":
					cleared_string = word.replace("Y", "")
					try:
						posY = float(cleared_string)
					except ValueError:
						pass
				if word[0] == "Z":
					try:
						cleared_string = word.replace("Z", "")
						posZ = int(cleared_string)
					except ValueError:
						pass
			#at the end of the line push to the position array the positions
			positions.append([posX,posY,posZ])
			progresBarMax += 1
		file.close()

	except TypeError:
		print("ERROR: can't open file")


def sendSerialData():
	global posX
	global posY
	global posZ
	global mode


	ser.write(bytes(mode + "\n",'utf-8'))
	ser.write(bytes(str(posX) + "\n",'utf-8'))
	ser.write(bytes(str(posY) + "\n",'utf-8'))
	ser.write(bytes(str(posZ) + "\n",'utf-8'))

def calibrate():      
	print("calibration")
	#send data over serial
	ser.write(bytes("HOME\n",'utf-8'))
	ser.write(bytes("0\n",'utf-8'))
	ser.write(bytes("0\n",'utf-8'))
	ser.write(bytes("0\n",'utf-8'))
	serialInput = ser.readline().decode('utf-8').rstrip()
	print(serialInput)

def gotoFunction():
	global posX 
	global posY

	posX = int(entryX.get())
	posY = int(entryY.get())

	positions.append([entryX.get(),entryY.get()])
	
	#send data over serial

	#send data over serial
	#TODO: change this to use the function
	ser.write(bytes("RAW" + "\n",'utf-8'))
	ser.write(bytes(str(posX) + "\n",'utf-8'))
	ser.write(bytes(str(posY) + "\n",'utf-8'))
	ser.write(bytes("0\n",'utf-8'))
	serialInput = ser.readline().decode('utf-8').rstrip()
	print(serialInput)

def clearDrawing():
	global positions
	positions = [[740/2,510,1]]

#run all the possitions until there are none left
def run():
	global mode
	global posX
	global posY
	global posZ
	global progresBar
	global progresBarMax

	#clear variables
	progresBar = 0

	file = open(filePath,"r")

	for line in file:
		for word in line.split():
			if word[0] == "(" or word[0] == ";":
				break#skips this loop if there is a comment

			if word == "G28":
				calibrate()
			elif word == "G0":
				mode = "RAPID"
			elif word == "G1":
				mode = "NORMAL"

			if word[0] == "X":
				cleared_string = word.replace("X", "")
				try:
					posX = float(cleared_string)
				except ValueError:
					pass
			if word[0] == "Y":
				cleared_string = word.replace("Y", "")
				try:
					posY = float(cleared_string)
				except ValueError:
					pass
			if word[0] == "Z":
				cleared_string = word.replace("Z", "")
				try:
					posZ = int(cleared_string)
				except ValueError:
					pass

		sendSerialData()
		serialInput = ser.readline().decode('utf-8').rstrip()

		while serialInput == "":
			print("waiting for input")
			serialInput = ser.readline().decode('utf-8').rstrip()

		while serialInput != "OK":
			#what's going on here?!
			if serialInput == "ERROR":
				#data got mangled up when sending it to the microcontroller, so it sent an error message
				print("ERROR: bad serial output")
			else:
				#data got corrupted on it's way in (i think this one is worse than the bad serial output)
				print("ERROR: bad serial input")
				print("INPUT: " + serialInput)

			sendSerialData()
			serialInput = ser.readline().decode('utf-8').rstrip()

		print("LINE: " + line)
		#print("SERIAL INPUT: " + serialInput)
		#progres bar that show how much procent until the end of the file
		progresBar += 1
		print("PROGRES: " + str(round(((progresBar/progresBarMax)*100),2)) + "%")

	file.close()

#create all the top bar buttons
calibrationButton = tk.Button(text='calibrate', command=calibrate)
Bplace.create_window(50, 20, window=calibrationButton)
gotoButton = tk.Button(text="goto",command=gotoFunction)
Bplace.create_window(130,20, window=gotoButton)
entryX = tk.Entry(root)
Bplace.create_window(250,20, window=entryX)
entryY = tk.Entry(root)
Bplace.create_window(350,20, window=entryY)
clearButton = tk.Button(text="clear",command=clearDrawing)
Bplace.create_window(450,20,window=clearButton)
runButton = tk.Button(text='run', command=run)
Bplace.create_window(510, 20, window=runButton)
openFileButton = tk.Button(text='open file',command=openFile)
Bplace.create_window(600,20,window=openFileButton)

def main():
	#clear the screen
	place.delete("all")
	#draw
	#background
	place.create_rectangle(0,0,root.winfo_width(),root.winfo_height(),fill='#101010')
	#debug cube
	place.create_rectangle((posX/screenSizeX*root.winfo_width())-5,(posY/screenSizeY*root.winfo_height())-5,(posX/screenSizeX*root.winfo_width())+5,(posY/screenSizeY*root.winfo_height())+5,fill='#fb1b1b')
	#draw lines on screen
	for i in range(len(positions)-1):
		if positions[i][2] == 1:
			place.create_line(positions[i][0]/screenSizeX*root.winfo_width(),positions[i][1]/screenSizeY*root.winfo_height(),positions[i+1][0]/screenSizeX*root.winfo_width(),positions[i+1][1]/screenSizeY*root.winfo_height(),fill="#A0A040",width=1)
		else:
			place.create_line(positions[i][0]/screenSizeX*root.winfo_width(),positions[i][1]/screenSizeY*root.winfo_height(),positions[i+1][0]/screenSizeX*root.winfo_width(),positions[i+1][1]/screenSizeY*root.winfo_height(),fill="#f0f0f0",width=1)

	root.after(2,main)

#place.bind('<Motion>',motion)
#place.bind("<Button-1>",onClick)
root.after(2,main)
root.mainloop()
