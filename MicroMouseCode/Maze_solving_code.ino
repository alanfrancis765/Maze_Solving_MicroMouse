// ===== Sensors =====
#define LEFT_SENSOR   2
#define FRONT_SENSOR  3
#define RIGHT_SENSOR  4

// ===== Motor Driver =====
#define PWMA 5
#define AIN1 8
#define AIN2 9

#define PWMB 6
#define BIN1 10
#define BIN2 11

#define STBY 7

#define SPEED 50
#define CORR 8

// ===== Setup =====
void setup() {
  Serial.begin(9600);// 

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(FRONT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
  initMaze();
  
  Serial.println("Place the Robot...");
  delay(4000);

  stopBot();
  delay(200);
}

int x = 0, y = 0;
int dir = 0; // 0=NORTH

int dist[7][8];
bool wall[7][8][4];

// x → horizontal
// y → vertical

int dx[4] = {0, +1, 0, -1};
int dy[4] = {+1, 0, -1, 0};


//Boundry  setup 
void initMaze() {

  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 8; j++) {
      dist[i][j] = abs(i - 3) + abs(j - 4);
    }
  }

  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 8; j++) {
      for (int d = 0; d < 4; d++) {
        wall[i][j][d] = false;
      }
    }
  }


  // Bottom & Top
  for (int i = 0; i < 7; i++) {
    setWall(i, 0, 2);   // SOUTH
    setWall(i, 7, 0);   // NORTH
  }

  // Left & Right
  for (int j = 0; j < 8; j++) {
    setWall(0, j, 3);   // WEST
    setWall(6, j, 1);   // EAST
  }
}

// Wall Update 
void setWall(int x, int y, int d) {
  wall[x][y][d] = true;

  int nx = x + dx[d];
  int ny = y + dy[d];

  if (nx >= 0 && nx < 7 && ny >= 0 && ny < 8) {
    wall[nx][ny][(d + 2) % 4] = true;
  }
}
void updateWalls(int left, int front, int right) {

  if (front == LOW)
    setWall(x, y, dir);

  if (left == LOW)
    setWall(x, y, (dir + 3) % 4);

  if (right == LOW)
    setWall(x, y, (dir + 1) % 4);
}

//Flood Fill Algorithm 
void floodFill() {

  bool changed = true;

  while (changed) {
    changed = false;

    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {

        int minVal = 255;

        for (int d = 0; d < 4; d++) {

          if (wall[i][j][d]) continue;

          int nx = i + dx[d];
          int ny = j + dy[d];

          if (nx >= 0 && nx < 7 && ny >= 0 && ny < 8) {
            minVal = min(minVal, dist[nx][ny]);
          }
        }

        if (minVal != 255 && dist[i][j] != minVal + 1) {
          dist[i][j] = minVal + 1;
          changed = true;
        }
      }
    }
  }
}

//Choose the best direction 
int chooseBestDir() {

  int priority[4] = {
    dir,
    (dir + 3) % 4,
    (dir + 1) % 4,
    (dir + 2) % 4
  };

  int bestDir = dir;
  int bestVal = 255;

  for (int i = 0; i < 4; i++) {
    int d = priority[i];

    if (wall[x][y][d]) continue;

    int nx = x + dx[d];
    int ny = y + dy[d];

    if (nx >= 0 && nx < 7 && ny >= 0 && ny < 8) {
      if (dist[nx][ny] < bestVal) {
        bestVal = dist[nx][ny];
        bestDir = d;
      }
    }
  }

  return bestDir;
}  

// Angle adjustment
void rotateTo(int targetDir) {

  int turn = (targetDir - dir + 4) % 4;

  if (turn == 1) turnRight();
  else if (turn == 3) turnLeft();
  else if (turn == 2) uTurn();

  dir = targetDir;
}

//Cell Movement 
void moveOneCell() {

  unsigned long start = millis();

  while (millis() - start < 320) {

    int left  = digitalRead(LEFT_SENSOR);
    int right = digitalRead(RIGHT_SENSOR);

    moveForwardCorrected(left, right);
  }

  stopBot();
  delay(50);

  x += dx[dir];
  y += dy[dir];
}

// ===== LOOP =====
void loop() {

  int left  = digitalRead(LEFT_SENSOR);
  int front = digitalRead(FRONT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  // Step 1: Update walls
  updateWalls(left, front, right);

  // Step 2: Flood fill
  floodFill();

  // Step 3: Choose direction
  int bestDir = chooseBestDir();

  // Step 4: Rotate
  rotateTo(bestDir);

  // Step 5: Move
  moveOneCell();

  //Just for the me to understant how the robot think
  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" DIR: "); Serial.println(dir);

  // Step 6: Goal check
  if (x == 3 && y == 4) {
    stopBot();
    delay(50);
    while (1);
  }
}

// ===== FORWARD WITH ALIGNMENT =====
void moveForwardCorrected(int left, int right) {

  int leftSpeed  = SPEED;
  int rightSpeed = SPEED;

  if (left == LOW && right == HIGH) {
    leftSpeed  = SPEED - CORR;
    rightSpeed = SPEED + CORR;
  }
  else if (right == LOW && left == HIGH) {
    leftSpeed  = SPEED + CORR;
    rightSpeed = SPEED - CORR;
  }

  // FORWARD (your correct direction)
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, leftSpeed);
  analogWrite(PWMB, rightSpeed);
}

// ===== STOP =====
void stopBot() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

// ===== TURNS =====

void turnLeft() {

  // LEFT backward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  // RIGHT forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, SPEED);
  analogWrite(PWMB, SPEED);

  delay(240);   // tuned
  stopBot();
}

void turnRight() {

  // LEFT forward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  // RIGHT backward
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, SPEED);
  analogWrite(PWMB, SPEED);

  delay(240);   // tuned
  stopBot();
}

//  FIXED U-TURN (REAL 180°)
void uTurn() {

  // LEFT forward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  // RIGHT backward
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, SPEED);
  analogWrite(PWMB, SPEED);

  delay(500);   // slightly reduced
  stopBot();
}

