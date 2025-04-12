import time
import random
import functools
import turtle


MAX_X = 600
MAX_Y = 800
DEFAULT_SIZE = 20
SNAKE_SHAPE = 'square'
HIGH_SCORES_FILE_PATH = "high_scores.txt"
# Controla a velocidade da cobra. Quanto menor o valor, mais rápido é o movimento da cobra.
SPEED = 0.5



def load_high_score(state):
    # se já existir um high score devem guardar o valor em state['high_score']
    write_high_score_to_file(state)
    ficheiro = open("high_scores.txt", 'r')
    numeros = ficheiro.readlines()
    maximo = max(numeros)
 
    ficheiro.close()
    state['high_score'] = int (maximo) 
 
    

def write_high_score_to_file(state):
    # devem escrever o valor que está em state['high_score'] no ficheiro de high scores
    ficheiro = open("high_scores.txt", 'a')
    ficheiro.write("%d \n" %state['high_score']) 
    ficheiro.close()
 
def create_score_board(state):
    score_board = turtle.Turtle()
    score_board.speed(0)
    score_board.shape("square")
    score_board.color("grey")
    score_board.penup()
    score_board.hideturtle()
    score_board.goto(0, MAX_Y / 2.2)
    state['score_board'] = score_board
    load_high_score(state)
    update_score_board(state)

def update_score_board(state):
    state['score_board'].clear()
    state['score_board'].write("Score: {} High Score: {}".format(state['score'], state['high_score']), align="center", font=("Helvetica", 24, "normal"))

def go_up(state):
    if state['snake']['current_direction'] != 'down':
        state['snake']['current_direction'] = 'up'

def go_down(state):
    if state['snake']['current_direction'] != 'up':
        state['snake']['current_direction'] = 'down'

def go_left(state):
    if state['snake']['current_direction'] != 'right':
        state['snake']['current_direction'] = 'left'

def go_right(state):
    if state['snake']['current_direction'] != 'left':
        state['snake']['current_direction'] = 'right'

def init_state():
    state = {}
    # Informação necessária para a criação do score board
    state['score_board'] = None
    state['new_high_score'] = False
    state['high_score'] = 0
    state['score'] = 0
    # Para gerar a comida deverá criar um nova tartaruga e colocar a mesma numa posição aleatória do campo
    state['food'] = None
    state['window'] = None
    snake = {
        'head': None,                  # Variável que corresponde à cabeça da cobra
        'current_direction': None, # Indicação da direcção atual do movimento da cobra
        'body': []
    }
    state['snake'] = snake
    return state

def setup(state):
    window = turtle.Screen()
    window.bgcolor("#F8F3C5")
    window.setup(width=MAX_X, height=MAX_Y)
    window.listen()
    window.onkey(functools.partial(go_up, state), 'w')
    window.onkey(functools.partial(go_down, state), 's')
    window.onkey(functools.partial(go_left, state), 'a')
    window.onkey(functools.partial(go_right, state), 'd')
    window.tracer(0)
    state['window'] = window
    snake = state['snake']
    snake['current_direction'] = 'stop'
    snake['head'] = turtle.Turtle()
    snake['head'].shape(SNAKE_SHAPE)
    snake['head'].showturtle()
    snake['head'].pu()
    snake['head'].color('green')
    create_score_board(state)
    create_food(state)

def move(state):
    ''' 
    Função responsável pelo movimento da cobra no ambiente.
    '''
    if state['snake']['current_direction'] == "up":
        y = state['snake']['head'].ycor()
        state['snake']['head'].sety(y+20)
    if state['snake']['current_direction'] == "down":
        y = state['snake']['head'].ycor()
        state['snake']['head'].sety(y-20) 
    if state['snake']['current_direction'] == "right":
        x = state['snake']['head'].xcor()
        state['snake']['head'].setx(x+20)  
    if state['snake']['current_direction'] == "left":
        x = state['snake']['head'].xcor()
        state['snake']['head'].setx(x-20)     
      
    
    snake = state['snake']

def create_food(state):
    ''' 
        Função responsável pela criação da comida. Note que elas deverão ser colocadas em posições aleatórias, mas dentro dos limites do ambiente.
    '''
  
    comida= turtle.Turtle()
    comida.shape('circle')
    comida.color('red')
    comida.turtlesize(0.5)
    comida.pu()
    xhead = state['snake']['head'].xcor()
    yhead = state['snake']['head'].ycor()
    x = random.randint((-MAX_X/2)+10,(MAX_X/2)-10)
    y = random.randint((-MAX_Y/2)+10,363) # 363 pois é o máximo onde o texto é apresentado
    comida.goto(x,y)
    comidax = comida.xcor()
    comiday = comida.ycor()
    while(xhead == comidax  and yhead == comiday):
        x = random.randint((-MAX_X/2)+10,(MAX_X/2)-10)
        y = random.randint((-MAX_Y/2)+10,363) # 363 pois é o máximo onde o texto é apresentado
        comida.goto(x,y)     
       
    state['food'] = comida 
   

    # a informação sobre a comida deve ser guardada em state['food']

def check_if_food_to_eat(state):
    ''' 
        Função responsável por verificar se a cobra tem uma peça de comida para comer. Deverá considerar que se a comida estiver a uma distância inferior a 15 pixels a cobra pode comer a peça de comida. 
    '''
       
    food = state['food']
    x = random.randint((-MAX_X/2)+10,(MAX_X/2)-10)
    y = random.randint((-MAX_Y/2)+10,363)
    update_score_board(state)
    if (state['snake']['head'].distance(food.pos()) < 15):
        food.goto(x,y)
        new_body = turtle.Turtle()
        new_body.color(("#855B13" if len(state['snake']['body']) % 2 else "#FACC2C"))
        new_body.shape('square')
        new_body.speed(0)
        new_body.pu()
        state['snake']['body'].append(new_body)
        global SPEED 
        SPEED -= 0.01
        state['score'] += 10
        if state['score'] > state['high_score']:
            state['high_score'] = state['score']
            state['new_high_score'] = state['high_score'] 
    for i in range(len(state['snake']['body'])-1,0,-1):
                x = state['snake']['body'][i-1].xcor()
                y = state['snake']['body'][i-1].ycor()
                state['snake']['body'][i].goto(x,y)     
    if len(state['snake']['body'])>0:
        xbody = state['snake']['head'].xcor()
        ybody = state['snake']['head'].ycor()
        state['snake']['body'][0].goto(xbody,ybody) 
        
        #para que?  
            
# para ler ou escrever os valores de high score, score e new high score, devem usar os respetivos campos do state: state['high_score'], state['score'] e state['new_high_score'](food state = food)
 
def boundaries_collision(state):
    ''' 
        Função responsável por verificar se a cobra colidiu com os limites do ambiente. Sempre que isto acontecer a função deverá returnar o valor booleano True, caso contrário retorna False.
    '''
    
    if (state['snake']['head'].xcor() < (-MAX_X/2) + 20 or state['snake']['head'].xcor() > (MAX_X/2) - 20 or state['snake']['head'].ycor() < (-MAX_Y/2) + 20 or state['snake']['head'].ycor() > MAX_Y/2 - 20):
        state['snake']['head'].goto(0,0)
        state['snake']['current_direction'] = 'stop'       
        state['score']=0
        turtle.bye()
             
        return True
    else:
        return False

def check_collisions(state):
    '''
        Função responsável por avaliar se há colisões. Atualmente apenas chama a função que verifica se há colisões com os limites do ambiente. No entanto deverá escrever o código para verificar quando é que a tartaruga choca com uma parede ou com o seu corpo.
    '''
    for quadradobody in state['snake']['body'] :
        if quadradobody.distance(state['snake']['head'])<20:
            turtle.bye()
            return True    
    
    snake = state['snake']
    return boundaries_collision(state)

def main():
    state = init_state()
    setup(state)
    while not check_collisions(state):
        state['window'].update()
        check_if_food_to_eat(state)
        move(state)
        time.sleep(SPEED)
    print("YOU LOSE!")
    
    if state['new_high_score']:
        write_high_score_to_file(state)
    
main()