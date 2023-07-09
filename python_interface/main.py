import tkinter as tk
import math
import serial
import time

#serial comunications init
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.reset_input_buffer()

screenSizeX = 740#mm
screenSizeY = 1000#mm
positions = [[0,0]]
mouseX = 0
mouseY = 0
posX = 0
posY = 0
posZ = 0

#tkinter init
root = tk.Tk()
root.title("robotic arm")
root.geometry("740x1000")
root.resizable(True,True)
Bplace=tk.Canvas(root,width=740,height=40)
Bplace.pack()
place=tk.Canvas(root,width=10,height=10)
place.pack(fill="both",expand=True)

#read from gcode file
file = open("drawing.gcode","r")

for line in file:
	for word in line.split():
		#TODO make G0 be a rapid mode and G1 lineary interpolated slow mode
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
	positions.append([posX,posY])

file.close()

def calibrate():      
    print("calibration")
	#send data over serial

def gotoFunction():
	global posX 
	global posY

	posX = int(entryX.get())
	posY = int(entryY.get())

	positions.append([entryX.get(),entryY.get()])
	
	#send data over serial

	#send data over serial
	ser.write(bytes("RAW" + "\n",'utf-8'))
	ser.write(bytes(str(posX) + "\n",'utf-8'))
	ser.write(bytes(str(posY) + "\n",'utf-8'))
	serialOutput = ser.readline().decode('utf-8').rstrip()
	print(serialOutput)

def clearDrawing():
	global positions
	positions = [[posX,posY]]

#run all the possitions until there are none left
def run():
	#send data over serial
	for i in range(len(positions)):
		#print(positions[i][0])
		ser.write(bytes(str(positions[i][0]) + "\n",'utf-8'))
		ser.write(bytes(str(positions[i][1]) + "\n",'utf-8'))

		serialOutput = ser.readline().decode('utf-8').rstrip()
		print(serialOutput)

		#show the debug cube
		#for word in serialOutput.split():
		#	posX = float(word)
		#	posY = float(word)

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

def motion(event):
	pass
	"""
	global mouseX
	global mouseY
	mouseX, mouseY = event.x,event.y
	"""
def onClick(event):
	pass
	#global mouseX
	#global mouseY
	#global posX
	#global posY
	#posX = mouseX*screenSizeX/root.winfo_width()
	#posY = mouseY*screenSizeY/root.winfo_width()
	#print(mouseX)
	#print(mouseY)
	#positions.append([mouseX,mouseY])

	#send data over serial
	#ser.write(bytes(str(posX) + "\n",'utf-8'))
	#ser.write(bytes(str(posY) + "\n",'utf-8'))
	#serialOutput = ser.readline().decode('utf-8').rstrip()
	#print(serialOutput)

def main():
	global posX
	global posY
	
	#clear the screen
	place.delete("all")
	#draw
	#background
	place.create_rectangle(0,0,root.winfo_width(),root.winfo_height(),fill='#101010')
	#debug cube
	place.create_rectangle((posX/screenSizeX*root.winfo_width())-5,(posY/screenSizeY*root.winfo_height())-5,(posX/screenSizeX*root.winfo_width())+5,(posY/screenSizeY*root.winfo_height())+5,fill='#fb1b1b')
	#draw lines
	for i in range(len(positions)-1):
		place.create_line(positions[i][0]/screenSizeX*root.winfo_width(),positions[i][1]/screenSizeY*root.winfo_height(),positions[i+1][0]/screenSizeX*root.winfo_width(),positions[i+1][1]/screenSizeY*root.winfo_height(),fill="#f0f0f0",width=2)

	root.after(2,main)

#root.bind('<KeyPress>', onKeyPress)
place.bind('<Motion>',motion)
place.bind("<Button-1>",onClick)
root.after(2,main)
root.mainloop()
