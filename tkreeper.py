from Tkinter import *
from subprocess import Popen, PIPE

#Enums
#Entities of the same color have the same number, for easy comparison
WHITE, BLACK = range(2) #Colors
WHITE, BLACK, EMPTY, DISABLED = range(4) #Pin entities
WHITE, BLACK, EMPTY, WHITE_BASE, BLACK_BASE= range(5) #Path entities
WHITE, BLACK, DRAW = range(3) #Outcomes

NAME = 'Tkreeper v.3a'

BOARD_SIZE = 12 * 8 * 6.5

ENGINE_PROCESS = None

class CreeperBoard(Canvas):
    def __init__(self, parent):        
        Canvas.__init__(self, parent,
                        width=BOARD_SIZE, height=BOARD_SIZE,
                        background='#653930', highlightthickness=0)
        self.pack()
        self.parent = parent
        
        #Octagon size
        size = (int(self.cget('width')) ) / 6.5
        half = size / 2
        quarter = size / 4
        eighth = size / 8

        #Draw path octagons
        self.path_board = [[] for _ in range(6)]
        for row in range(6):
            for col in range(6):
                points = []
                
                x = quarter + (size * col) + quarter
                y = quarter + (size * row)
                if col == 0 and row == 0:
                    x = quarter
                points.append((x, y))
                
                x = quarter + (size * col) + half + quarter
                y = quarter + (size * row)
                if col == 5 and row == 0:
                    x = quarter + (size * col) + size
                points.append((x, y))
                
                x = quarter + (size * col) + size
                y = quarter + (size * row) + quarter
                points.append((x, y))
                
                x = quarter + (size * col) + size
                y = quarter + (size * row) + half + quarter
                points.append((x, y))
                
                x = quarter + (size * col) + half + quarter
                y = quarter + (size * row) + size
                if col == 5 and row == 5:
                    x = quarter + (size * col) + size
                points.append((x, y))
                
                x = quarter + (size * col) + quarter
                y = quarter + (size * row) + size
                if col == 0 and row == 5:
                    x = quarter
                points.append((x, y))
                
                x = quarter + (size * col)
                y = quarter + (size * row) + half + quarter
                points.append((x, y))
                
                x = quarter + (size * col)
                y = quarter + (size * row) + quarter
                points.append((x, y))
                
                self.path_board[row].append(
                    self.create_polygon(points, outline='#000', width=2, fill='#ffb380'))
        
        #Mapping of pin objects to names
        self.pin_names = {}

        #Draw pin circles
        self.pin_board = [[] for _ in range(7)]
        self.pin_board[0].append(None)
        self.pin_board[6].append(None)
        for row in range(7):
            for col in range(7):
                if (row == 0 or row == 6) and (col == 0 or col == 6):
                    continue
                
                x1 = size*col + eighth
                y1 = size*row + eighth
                x2 = size*col + quarter + eighth
                y2 = size*row + quarter + eighth
                pin = self.create_oval((x1, y1), (x2, y2),
                                       width=2, fill='#999')
                
                self.pin_board[row].append(pin)
                
                pin_name = 'ABCDEFG'[col] + str(7-row)
                self.pin_names[pin] = pin_name
                
                self.tag_bind(pin, '<Button-1>', self.pin_click_handler(pin))

        self.pin_board[0].append(None)
        self.pin_board[6].append(None)
        
        self.bind('<Button-2>', self.deselect)
        
        self.engine = Engine()
        self.updateBoard()
                
    def updateBoard(self):
        self.engine.send('getboard')
        board = list(self.engine.listen())
        
        def drawPin(x, y, color):
            y = 6-y
            
            if color == 'w':
                fill = '#fff'
            elif color == 'b':
                fill = '#222'
            else:
                fill = '#999'
            
            self.itemconfigure(self.pin_board[y][x], fill=fill)
        
        def drawPath(x, y, color):
            y = 5-y
            
            if color == 'w':
                fill = '#ddd'
            elif color == 'b':
                fill = '#333'
            else:
                return
            
            self.itemconfigure(self.path_board[y][x], fill=fill)
        
        drawPath(0, 0, 'b')
        drawPath(5, 5, 'b')
        drawPath(0, 5, 'w')
        drawPath(5, 0, 'w')
        
        for mx in range(5):
            drawPin(1+mx, 0, board.pop(0))
        
        for my in range(5):
            for x in range(7):
                drawPin(x, 1+my, board.pop(0))
        
        for mx in range(5):
            drawPin(1+mx, 6, board.pop(0))
    
        #Update path board
        for mx in range(4):
            drawPath(1+mx, 0, board.pop(0))
        
        for my in range(4):
            for x in range(6):
                drawPath(x, 1+my, board.pop(0))
         
        for mx in range(4):
            drawPath(1+mx, 5, board.pop(0))
                
        turn = board.pop()
        if turn == 'w':
            self.turn = WHITE
        elif turn == 'b':
            self.turn = BLACK
        
        if self.turn == WHITE:
            self.parent.title(NAME + ' - White to Move')
        elif self.turn == BLACK:
            self.parent.title(NAME + ' - Black to Move')
        
        self.engine.send('getwinner')
        winner = self.engine.listen()
        
        self.winner = None
        if winner == 'w':
            self.winner = WHITE
        elif winner == 'b':
            self.winner = BLACK
        
        if self.winner == WHITE:
            self.parent.title(NAME + ' - White Wins!')
        elif self.winner == BLACK:
            self.parent.title(NAME + ' - Black Wins!')
        elif self.winner == DRAW:
            self.parent.title(NAME + ' - Draw.')
        
        #Get possible moves
        self.moves = []
        self.engine.send('getmoves')
        move = self.engine.listen()
        while move != 'done':
            self.moves.append(move)
            move = self.engine.listen()
        
        self.selected = None
        
        if self.turn == BLACK and self.winner == None:
            self.parent.after(250, self.computer_move) 
    
    def computer_move(self):
        self.engine.send('think')
        move = self.engine.listen()
        self.engine.send('move ' + move)
        self.updateBoard()
        
    def pin_click_handler(self, pin):
        def pin_click(event):
            if self.winner != None:
                return
            
            pin_name = self.pin_names[pin]
            
            if self.selected == pin:
                self.deselect()
                return
            
            validStart = False
            for move in self.moves:
                if move.startswith(pin_name):
                    validStart = True
                    break
            
            if validStart:
                self.deselect()
                self.selected = pin
                self.itemconfigure(pin, outline='blue')
                return
            
            if self.selected != None:
                move = self.pin_names[self.selected] + '-' + pin_name
                if move in self.moves:
                    self.deselect()
                    self.engine.send('move ' + move)
                    self.updateBoard()
                            
        return pin_click
    
    def deselect(self, event=None):
        if self.selected != None:
            self.itemconfigure(self.selected, outline='#000')
        self.selected = None
    
    def destroy(self):
        self.engine.stop()
        Canvas.destroy(self)
    
class Engine(object):
    def __init__(self):
        self.__process = Popen('bin/pilgrim', stdin=PIPE, stdout=PIPE)
        
        global ENGINE_PROCESS
        ENGINE_PROCESS = self.__process
        
        self.listen()
    
    def listen(self):
        line = self.__process.stdout.readline().strip()
        #print line
        return line
    
    def send(self, line):
        self.__process.stdin.write(line + '\n')
    
    def stop(self):
        self.send('stop')

def main():
    root = Tk()
    root.title(NAME)
    root.resizable(0,0)
    
    CreeperBoard(root)
    
    root.mainloop()
        
if __name__ == '__main__':
    main()
    ENGINE_PROCESS.kill()
