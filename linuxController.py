import tkinter as tk
import math
import serial
import time

#serial comunications init
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.reset_input_buffer()

posX = 500
posY = 10
screenSizeX = 1000#mm
screenSizeY = 1000#mm
positions = [[500,10]]
mouseX = 0
mouseY = 0

#tkinter init
root = tk.Tk()
root.title("robotic arm")
root.geometry("1000x1000")
root.resizable(True,True)
Bplace=tk.Canvas(root,width=1000,height=40)
Bplace.pack()
place=tk.Canvas(root,width=10,height=10)
place.pack(fill="both",expand=True)


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

calibrationButton = tk.Button(text='calibrate', command=calibrate)
Bplace.create_window(50, 20, window=calibrationButton)
gotoButton = tk.Button(text="goto",command=gotoFunction)
Bplace.create_window(100,20, window=gotoButton)
entryX = tk.Entry(root) 
Bplace.create_window(200,20, window=entryX)
entryY = tk.Entry(root) 
Bplace.create_window(300,20, window=entryY)
clearButton = tk.Button(text="clear",command=clearDrawing)
Bplace.create_window(400,20,window=clearButton)

def motion(event):
	global mouseX
	global mouseY
	mouseX, mouseY = event.x,event.y

def onClick(event):
	global mouseX
	global mouseY
	global posX
	global posY
	posX = mouseX*screenSizeX/root.winfo_width()
	posY = mouseY*screenSizeY/root.winfo_width()
	print(mouseX)
	print(mouseY)
	positions.append([mouseX,mouseY])

	#send data over serial
	ser.write(bytes("NORMAL" + "\n",'utf-8'))
	ser.write(bytes(str(posX) + "\n",'utf-8'))
	ser.write(bytes(str(posY) + "\n",'utf-8'))
	serialOutput = ser.readline().decode('utf-8').rstrip()
	print(serialOutput)

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