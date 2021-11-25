#define _CRT_SECURE_NO_WARNINGS
#include <gl/glut.h>
#include "utils.h"
#include "Shape.h"
#include <stdio.h>
#include <vector>
#include <queue>
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <algorithm>

enum { LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80, EASY = 10, MEDIUM = 5, HARD = 2};

bool finish = false; float step = 1;

using namespace GraphUtils;

class Board : public Shape {
public:
	Board(float xCenter, float yCenter, float zCenter,
		float xSize, float ySize, float zSize,
		float* diffColor, float* ambiColor, float* specColor)
		: Shape(xCenter, yCenter, zCenter,
			xSize, ySize, zSize,
			specColor, diffColor, ambiColor) { }
	virtual void draw() {
		glMaterialfv(GL_FRONT, GL_AMBIENT, getAmbiColor());
		glMaterialfv(GL_FRONT, GL_DIFFUSE, getDiffColor());
		glMaterialfv(GL_FRONT, GL_SPECULAR, getSpecColor());
		glMaterialf(GL_FRONT, GL_SHININESS, GraphUtils::shininess);
		// ����� ������� ������� � ����
		// (���������� ����� ������� ������� ��� ���������� ������������):
		glPushMatrix();
		glTranslatef(getXCenter(), getYCenter(), getZCenter());
		GraphUtils::parallelepiped(getXSize(), getYSize(), getZSize());
		// ³��������� ������� ������� � �����:
		glPopMatrix();
	}
};

class Block : public Shape {
public:
	Block(float xCenter, float yCenter, float zCenter,
		float xSize, float ySize, float zSize,
		float* diffColor, float* ambiColor, float* specColor)
		: Shape(xCenter, yCenter, zCenter,
			xSize, ySize, zSize,
			specColor, diffColor, ambiColor) { }
	virtual void draw() {
		glMaterialfv(GL_FRONT, GL_AMBIENT, getAmbiColor());
		glMaterialfv(GL_FRONT, GL_DIFFUSE, getDiffColor());
		glMaterialfv(GL_FRONT, GL_SPECULAR, getSpecColor());
		glMaterialf(GL_FRONT, GL_SHININESS, GraphUtils::shininess);
		// ����� ������� ������� � ����
		// (���������� ����� ������� ������� ��� ���������� ������������):
		glPushMatrix();
		glTranslatef(getXCenter(), getYCenter(), getZCenter());
		GraphUtils::parallelepiped(getXSize(), getXSize(), getXSize());
		// ³��������� ������� ������� � �����:
		glPopMatrix();
	}
};

class Coin : public Shape
{
private:
	float innerRadius, angle = 0.0f;
public:
	Coin(float xCenter, float yCenter, float zCenter,
		float xSize, float ySize, float zSize,
		float* diffColor, float* ambiColor, float* specColor,
		float innerRadius)
		: Shape(xCenter, yCenter, zCenter, xSize, ySize, zSize,
			specColor, diffColor, ambiColor) {
		this->innerRadius = innerRadius;
	}
	float getInnerRadius() const { return innerRadius; }
	void setInnerRadius(float innerRadius) { this->innerRadius = innerRadius; }
	virtual void draw() {
		// ���������� ������������ ��������:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, getAmbiColor());
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, getDiffColor());
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, getSpecColor());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, GraphUtils::shininess);
		// ����� ������� ������� � ����
		// (���������� ����� ������� ������� ��� ���������� ������������):
		angle += 3.0f;
		glPushMatrix();
		glTranslatef(getXCenter(), getYCenter() + getYSize() / 2, getZCenter());
		// ������ ������� ���� ������������ � ������������� ��������:
		glRotatef(90, 1, 0, 0);
		glRotatef(angle, 0, -5, 0);
		GLUquadricObj* quadricObj = gluNewQuadric();
		gluCylinder(quadricObj, getXSize() / 2, getXSize() / 2, getYSize(), 20, 2);
		// ���� ������� ���� ������������ ��������� ������ ������:
		glRotatef(180, 1, 0, 0);
		// ������� ���� ������:
		gluDisk(quadricObj, innerRadius, getXSize() / 2, 20, 20);
		// ������� ���� �����:
		glTranslatef(0, 0, -getYSize());
		gluDisk(quadricObj, innerRadius, getXSize() / 2, 20, 20);
		gluDeleteQuadric(quadricObj);
		// ³��������� ������� ������� � �����:
		glPopMatrix();
	};
};

#define SnakeTail snake[snake.size() - 1]
#define SnakeHead snake->GetSnake()[0]
#define SnTail snake->GetSnake()[snake->GetSnake().size() - 1]

class Snake {
	std::vector <Block*> snake;
	std::pair <int, int> head;
	std::pair <int, int> tail;
	int head_dir, tail_dir;

public:
	Snake() {
		snake.push_back(new Block(0, 0.5, 0, 1, 0.09, 1, diffRed, ambiRed, specRed));
		head.first = 0; head.second = 0;
		snake.push_back(new Block(SnakeTail->getXCenter() - step, 0.5, 0.0, 1, 0.09, 1, diffGreen, ambiGreen, specGreen));
		tail.first = SnakeTail->getXCenter(); tail.second = SnakeTail->getZCenter();
		head_dir = RIGHT, tail_dir = RIGHT;
	}
	~Snake() {
		for (int i = 0; i < snake.size(); i++)
		{
			delete snake[i];
		}
	}
	std::vector<Block*>& GetSnake() { return snake; }
	int GetDirection_H() { return head_dir; }
	int GetDirection_T() { return tail_dir; }
	std::pair <int, int>& GetTail() { return tail; }
	std::pair <int, int>& GetHead() { return head; }
	void SetDirection_H(int dir) { head_dir = dir; }
	void SetDirection_T(int dir) { tail_dir = dir; }
	int GetDirH() { return head_dir; }
	int GetDirT() { return tail_dir; }
	/*void Eat() {
		if (tail_dir == RIGHT)
			snake.push_back(new Block(SnakeTail->getXCenter() - step, SnakeTail->getYCenter() , SnakeTail->getZCenter(), 1, 0.09, 1, diffGreen, ambiGreen, specGreen));
		else if (tail_dir == LEFT)
			snake.push_back(new Block(SnakeTail->getXCenter() + step, SnakeTail->getYCenter(), SnakeTail->getZCenter(), 1, 0.09, 1, diffGreen, ambiGreen, specGreen));
		else if(tail_dir == UP)
			snake.push_back(new Block(SnakeTail->getXCenter(), SnakeTail->getYCenter(), SnakeTail->getZCenter() + step, 1, 0.09, 1, diffGreen, ambiGreen, specGreen));
		else if(tail_dir == DOWN)
			snake.push_back(new Block(SnakeTail->getXCenter(), SnakeTail->getYCenter(), SnakeTail->getZCenter() - step, 1, 0.09, 1, diffGreen, ambiGreen, specGreen));
		tail.first = SnakeTail->getXCenter() - step; tail.second = SnakeTail->getZCenter();
	}*/

	bool IsOnSnake(std::pair<int, int> coords) {
		bool Is = false;
		for (int i = 0; i < snake.size(); i++)
		{
			if (coords.first == snake[i]->getXCenter() and coords.second == snake[i]->getZCenter())
				Is = true;
		}
		return Is;
	}
	void draw() {
		for (int i = 0; i < snake.size(); i++) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, snake[i]->getAmbiColor());
			glMaterialfv(GL_FRONT, GL_DIFFUSE, snake[i]->getDiffColor());
			glMaterialfv(GL_FRONT, GL_SPECULAR, snake[i]->getSpecColor());
			glMaterialf(GL_FRONT, GL_SHININESS, GraphUtils::shininess);
			// ����� ������� ������� � ����
			// (���������� ����� ������� ������� ��� ���������� ������������):
			glPushMatrix();
			glTranslatef(snake[i]->getXCenter(), snake[i]->getYCenter(), snake[i]->getZCenter());
			GraphUtils::parallelepiped(snake[i]->getXSize(), snake[i]->getXSize(), snake[i]->getXSize());
			// ³��������� ������� ������� � �����:
			glPopMatrix();
		}
	}
};

bool decrease(std::pair<int, std::string> a, std::pair<int, std::string> b) { return a.first > b.first; }

class Scene {
	bool NameEnter = true, StartScreen = false, pause = false, Walls = true;

	std::vector<Shape*> shapes;
	std::vector<Shape*> foods;
	int score = 0;
	Snake* snake;

	float width, height;
	float distZ;
	float angleX, angleY;
	int button;
	float mouseX, mouseY;

	int dir;
	std::queue <int> RotDir;
	std::queue <std::pair<int, int>> rotates;
	Coin* food;
	std::pair<int, int> FoodPlace;

	char top[128], text[128], text1[128], text2[128], text3[128], 
		text4[128], text5[128], text6[128], text7[128], text8[128], 
		text9[128], text10[128], text11[128], text12[128], text13[128], text14[128],
		slow[6] = "easy:", mid[10] = "medium:", hard[10] = "hard:";
	bool NeedScore = false, ShowScores = false;

	std::vector<Shape*> btns;
	int hard_mode = 10;

	int tick = 0;
	int count = 0; //������ �� ��������� �������
	bool BP1 = true, BP2 = false, BP3 = false;

	std::vector<char> name; char nm[128];
	std::vector<std::pair<int, std::string>> scores, scores1, scores2;
public:
	Scene() {
		shapes.push_back(new Board(0.0, 0.0, 0.0, 26, 0.07, 26, diffViolet, ambiViolet, specViolet)); //������ ����� �� �������
		shapes.push_back(new Board(12.5, 1, 0.0, 1, 2, 26, diffYellow, ambiYellow, specYellow));
		shapes.push_back(new Board(-12.5, 1, 0.0, 1, 2, 26, diffYellow, ambiYellow, specYellow));
		shapes.push_back(new Board(0.0, 1, 12.5, 24, 2, 1, diffYellow, ambiYellow, specYellow));
		shapes.push_back(new Board(0.0, 1, -12.5, 24, 2, 1, diffYellow, ambiYellow, specYellow));

		btns.push_back(new Board(12.5, 12, 0.0, 1, 2, 2, diffYellow, ambiYellow, specYellow)); //������ ������ �� ��������� �����
		btns.push_back(new Board(12.5, 12, 3.5, 0.1, 2, 2, diffRed, ambiRed, specRed));
		btns.push_back(new Board(12.5, 12, 7, 0.1, 2, 2, diffYellow, ambiYellow, specYellow));
		btns.push_back(new Board(12.5, 12, 10.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow));
	}
	~Scene() {
		for (int i = 0; i < shapes.size(); i++) {
			delete shapes[i];
		}
		for (int i = 0; i < foods.size(); i++)
		{
			delete foods[i];
		}
		for (int i = 0; i < btns.size(); i++)
		{
			delete btns[i];
		}
		delete snake;
	}
	bool IsPause() { return pause; }
	bool GetState() {return NameEnter;}
	void SetState(bool state) {
		if (!name.empty()) {
			NameEnter = state;
			StartScreen = true;
			name.push_back('!');
			for (int i = 0; i < name.size(); i++)
			{
				nm[i] = name[i];
			}
		}
	}
	void DelName() {
		if (!name.empty()) {
			text8[name.size() - 1] = ' ';
			name.pop_back();
		}
	}
	void SetName(char letter){
		name.push_back(letter);
		for (int i = 0; i < name.size(); i++)
		{
			text8[i] = name[i];
		}
	}
	void initialize() //����������� ���������� ��������� ���
	{
		distZ = -25;
		angleX = 0;
		angleY = 75;
		dir = RIGHT;
		score = 0;
		snake = new Snake();
		finish = false;
		NeedScore = true;
		StartScreen = false;
		pause = true;
		ShowScores = false;
		foods.clear();
		EmplaceFood();
	}
	void initStart() { //��������� ���������� ������
		distZ = -40;
		angleX = -75.75;
		angleY = 90;
	}
	void WriteScore() { //����� ������� �� ����� ������ ����������(3)
		std::fstream best_scores;
		std::pair<int, std::string> tmp;
		if (NeedScore) {
			name.pop_back();
			best_scores.open("scores.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			if (score > 0) {
				if(!best_scores.eof())
					best_scores << std::endl;
				best_scores << hard_mode << "  " << score << "  ";
				for (auto el : name) {
					best_scores << el;
				}
			}
			best_scores.clear(); best_scores.seekg(0);
			int temp = 0, temp1 = 0; std::string temp2;
			while (!best_scores.eof()) {
				best_scores >> temp1;
				if (EASY == temp1) {
					best_scores >> temp;
					best_scores >> temp2;
					if (!temp2.empty()) {
						tmp.first = temp; 
						tmp.second = temp2;
						tmp.second.append(" - ");
						scores.push_back(tmp);
					}
					temp2.clear(); 
				}
				else if (MEDIUM == temp1) {
					best_scores >> temp;
					best_scores >> temp2;
					if (!temp2.empty()) {
						tmp.first = temp;
						tmp.second = temp2;
						tmp.second.append(" - ");
						scores1.push_back(tmp);
					}
					temp2.clear();
				}
				else if (HARD == temp1) {
					best_scores >> temp;
					best_scores >> temp2;
					if (!temp2.empty()) {
						tmp.first = temp;
						tmp.second = temp2;
						tmp.second.append(" - ");
						scores2.push_back(tmp);
					}
					temp2.clear();
				}
				else {
					best_scores.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				
			}

			sort(scores.begin(), scores.end(), decrease);
			sort(scores1.begin(), scores1.end(), decrease);
			sort(scores2.begin(), scores2.end(), decrease);


			if (scores.size() > 0) {
				for (int i = 0; i < scores[0].second.size(); i++)
				{
					text5[i] = scores[0].second[i];
				}
				for (int i = 0; i < std::to_string(scores[0].first).size(); i++)
				{
					text5[i + scores[0].second.size()] = std::to_string(scores[0].first)[i];
				}
			}
			if (scores.size() > 1) {
				for (int i = 0; i < scores[1].second.size(); i++)
				{
					text6[i] = scores[1].second[i];
				}
				for (int i = 0; i < std::to_string(scores[1].first).size(); i++)
				{
					text6[i + scores[1].second.size()] = std::to_string(scores[1].first)[i];
				}
			}
			if (scores.size() > 2) {
				for (int i = 0; i < scores[2].second.size(); i++)
				{
					text7[i] = scores[2].second[i];
				}
				for (int i = 0; i < std::to_string(scores[2].first).size(); i++)
				{
					text7[i + scores[2].second.size()] = std::to_string(scores[2].first)[i];
				}
			}



			if (scores1.size() > 0) {
				for (int i = 0; i < scores1[0].second.size(); i++)
				{
					text9[i] = scores1[0].second[i];
				}
				for (int i = 0; i < std::to_string(scores1[0].first).size(); i++)
				{
					text9[i + scores1[0].second.size()] = std::to_string(scores1[0].first)[i];
				}
			}
			if (scores1.size() > 1) {
				for (int i = 0; i < scores1[1].second.size(); i++)
				{
					text10[i] = scores1[1].second[i];
				}
				for (int i = 0; i < std::to_string(scores1[1].first).size(); i++)
				{
					text10[i + scores1[1].second.size()] = std::to_string(scores1[1].first)[i];
				}
			}
			if (scores1.size() > 2) {
				for (int i = 0; i < scores1[2].second.size(); i++)
				{
					text11[i] = scores1[2].second[i];
				}
				for (int i = 0; i < std::to_string(scores1[2].first).size(); i++)
				{
					text11[i + scores1[2].second.size()] = std::to_string(scores1[2].first)[i];
				}
			}



			if (scores2.size() > 0) {
				for (int i = 0; i < scores2[0].second.size(); i++)
				{
					text12[i] = scores2[0].second[i];
				}
				for (int i = 0; i < std::to_string(scores2[0].first).size(); i++)
				{
					text12[i + scores2[0].second.size()] = std::to_string(scores2[0].first)[i];
				}
			}
			if (scores2.size() > 1) {
				for (int i = 0; i < scores2[1].second.size(); i++)
				{
					text13[i] = scores2[1].second[i];
				}
				for (int i = 0; i < std::to_string(scores2[1].first).size(); i++)
				{
					text13[i + scores2[1].second.size()] = std::to_string(scores2[1].first)[i];
				}
			}
			if (scores2.size() > 2) {
				for (int i = 0; i < scores2[2].second.size(); i++)
				{
					text14[i] = scores2[2].second[i];
				}
				for (int i = 0; i < std::to_string(scores2[2].first).size(); i++)
				{
					text14[i + scores2[2].second.size()] = std::to_string(scores2[2].first)[i];
				}
			}

			NeedScore = false;
		}
		best_scores.close();
	}
	void MoveCoin(int dir) {
		if (dir == RIGHT) {
			if(!snake->IsOnSnake(std::make_pair(foods[0]->getXCenter() + step, foods[0]->getZCenter())) and foods[0]->getXCenter() + step != 12)
				foods[0]->setXCenter(foods[0]->getXCenter() + step);
		}
		else if (dir == LEFT) {
			if (!snake->IsOnSnake(std::make_pair(foods[0]->getXCenter() - step, foods[0]->getZCenter())) and foods[0]->getXCenter() - step != -12)
				foods[0]->setXCenter(foods[0]->getXCenter() - step);
		}
		else if (dir == UP) {
			if (!snake->IsOnSnake(std::make_pair(foods[0]->getXCenter(), foods[0]->getZCenter() - step)) and foods[0]->getZCenter() - step != -12)
				foods[0]->setZCenter(foods[0]->getZCenter() - step);
		}
		else if (dir == DOWN) {
			if (!snake->IsOnSnake(std::make_pair(foods[0]->getXCenter(), foods[0]->getZCenter() + step)) and foods[0]->getZCenter() + step != 12)
				foods[0]->setZCenter(foods[0]->getZCenter() + step);
		}
		FoodPlace.first = foods[0]->getXCenter(); FoodPlace.second = foods[0]->getZCenter();
	}
	void move() {
		Block* temp;
		if (snake->GetDirection_H() == RIGHT) {
			SnakeHead->setXCenter(SnakeHead->getXCenter() + step);
			temp = new Block(SnakeHead->getXCenter() - step, 0.5, SnakeHead->getZCenter(), 1, 0.09, 1, diffGreen, ambiGreen, specGreen);
		}
		else if (snake->GetDirection_H() == LEFT) {
			SnakeHead->setXCenter(SnakeHead->getXCenter() - step);
			temp = new Block(SnakeHead->getXCenter() + step, 0.5, SnakeHead->getZCenter(), 1, 0.09, 1, diffGreen, ambiGreen, specGreen);
		}
		else if (snake->GetDirection_H() == UP) {
			SnakeHead->setZCenter(SnakeHead->getZCenter() - step);
			temp = new Block(SnakeHead->getXCenter(), 0.5, SnakeHead->getZCenter() + step, 1, 0.09, 1, diffGreen, ambiGreen, specGreen);
		}
		else if (snake->GetDirection_H() == DOWN) {
			SnakeHead->setZCenter(SnakeHead->getZCenter() + step);
			temp = new Block(SnakeHead->getXCenter(), 0.5, SnakeHead->getZCenter() - step, 1, 0.09, 1, diffGreen, ambiGreen, specGreen);
		}

	
		if (Walls) { //����� ����
			if (SnakeHead->getXCenter() == 12 or SnakeHead->getXCenter() == -12 or SnakeHead->getZCenter() == 12 or SnakeHead->getZCenter() == -12
				or snake->IsOnSnake(snake->GetHead())) { //���� ��������� � ������ ��� � ����
				finish = true;
				WriteScore();
			}
		}
		else {
			if (snake->IsOnSnake(snake->GetHead())) {
				finish = true;
				WriteScore();
			}

			if (SnakeHead->getXCenter() == 13 or SnakeHead->getXCenter() == -13) {
				SnakeHead->setXCenter(SnakeHead->getXCenter() * -1);
			}
			if (SnakeHead->getZCenter() == 13 or SnakeHead->getZCenter() == -13) {
				SnakeHead->setZCenter(SnakeHead->getZCenter() * -1);
			}
		}

		snake->GetSnake().insert(snake->GetSnake().begin() + 1, temp);

		if (snake->GetHead() != FoodPlace) { //if ne ela
			snake->GetSnake().pop_back();
		}
		else { //if ela
			foods.pop_back();
			EmplaceFood();
			score++;
		}
		snake->GetHead().first = SnakeHead->getXCenter(); snake->GetHead().second = SnakeHead->getZCenter();
		snake->GetTail().first = SnTail->getXCenter(); snake->GetTail().second = SnTail->getZCenter();
		//std::cout << SnakeHead->getXCenter() << "  " << SnakeHead->getZCenter() << std::endl;
	}
	void setdir(int dir) { this->dir = dir; } //��������� �������� ����
	void EmplaceFood() { //��������� �� �� �����
		int temp, znak;
		do {
			temp = rand() % 12; znak = rand() % 2;
			FoodPlace.first = znak == 1 ? temp : temp * -1;
			temp = rand() % 12; znak = rand() % 2;
			FoodPlace.second = znak == 1 ? temp : temp * -1;
		} while (snake->IsOnSnake(FoodPlace));
		food = new Coin(FoodPlace.first, 0.4, FoodPlace.second, 1.1, 0.3, 1, diffYellow, ambiYellow, specYellow, 0);
		foods.push_back(food);
	}
	int GetScore() { return score; }
	void ClearScore() {
		std::fstream ofs;
		ofs.open("scores.txt", std::ios::out | std::ios::trunc);
		ofs.close();
		sprintf(text5, "                                  ");
		sprintf(text6, "                                  ");
		sprintf(text7, "                                  ");
		sprintf(text9, "                                  ");
		sprintf(text10, "                                 ");
		sprintf(text11, "                                 ");
		sprintf(text12, "                                 ");
		sprintf(text13, "                                 ");
		sprintf(text14, "                                 ");
	}
	bool TurnCheck(int direction) { //�������� �� ��, �� ������� �������� �������
		if ((direction == LEFT or direction == RIGHT) and (snake->GetDirection_H() == UP or snake->GetDirection_H() == DOWN)) { //������. ����. �������.�������
			snake->SetDirection_H(direction);
			return true;
		}
		else if ((direction == UP or direction == DOWN) and (snake->GetDirection_H() == LEFT or snake->GetDirection_H() == RIGHT)) {//�����. ����. ������. �������
			snake->SetDirection_H(direction);
			return true;
		}
		return false;
	}
	void TurnSnake(int direction) { //��������� ��������
		if (TurnCheck(direction)) {
			rotates.push(snake->GetHead());
			RotDir.push(direction);
		}
		if (!rotates.empty() and snake->GetTail() == rotates.front()) {
			rotates.pop();
			snake->SetDirection_T(RotDir.front());
			RotDir.pop();
		}
	}
	void on_size(int width, int height) { //������ ����
		this->width = width;
		if (height == 0)
			height = 1;
		this->height = height;
	} 
	void on_paint() { //������� ��������� �����
		if (NameEnter) {
			sprintf(text, "Enter your name: ");
		}
		else if (StartScreen) {
			initStart();
			sprintf(text, "Hello, ");
			sprintf(text1, "Choose your game options (F2 - start game   ESC - exit game)");
			sprintf(top, "- walls disabling option");
			sprintf(text2, "- slow speed");
			sprintf(text3, "- medium speed");
			sprintf(text4, "- high speed");
		}
		else if (finish) {
			sprintf(text, "Game over. Your score: %d    ", score);
			sprintf(text1, "Esc - exit    F3 - restart    F4 - show best scores    F7 - clear scores");
			sprintf(top, "Best scores: ");
		}
		else if(!finish) {
			sprintf(text, pause ? "SCORE: %d   Esc - exit   F3 - restart   F1 - start" : "SCORE: %d   Esc - exit   F3 - restart" , score);
		}
		// ������������ ������� ��������� ����, ��� ���� �������� ��� ����:
		glViewport(0, 0, width, height);

		// ���������� ��������� �������� � ������� �����:
		float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ���� �������� ��������� 
		float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���� ��������� ��������� 
		float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// ���� ������������ �����������
		float lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };// ������������ ������� �����
		
		//��������� ������� �����:
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		// ���� ��� ��������:
		if (finish)
		{
			glClearColor(0, 0.7, 0.7, 0);
		}
		else
		{
			glClearColor(0, 0.5, 0.5, 0);
		}

		// ������� ������:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// ����������� ������
		if (NameEnter) {
			glOrtho(0, 1, 0, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glColor3f(1, 1, 0); // ������ �����
			drawString(GLUT_BITMAP_HELVETICA_18, text, 0.43, 0.65);
			drawString(GLUT_BITMAP_HELVETICA_18, text8, 0.4, 0.6);
			glPopMatrix();
		}
		else if (StartScreen) {
			glOrtho(0, 1, 0, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glColor3f(1, 1, 0); // ������ �����
			drawString(GLUT_BITMAP_HELVETICA_18, text, 0.45, 0.65);
			drawString(GLUT_BITMAP_HELVETICA_18, nm, 0.51, 0.65);
			drawString(GLUT_BITMAP_HELVETICA_18, text1, 0.2, 0.6);
			drawString(GLUT_BITMAP_HELVETICA_18, top, 0.32, 0.49);
			drawString(GLUT_BITMAP_HELVETICA_18, text2, 0.32, 0.37);
			drawString(GLUT_BITMAP_HELVETICA_18, text3, 0.32, 0.25);
			drawString(GLUT_BITMAP_HELVETICA_18, text4, 0.32, 0.13);

			glPopMatrix();

		}
		else if (!finish) {
			glOrtho(0, 1, 0, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glColor3f(1, 1, 0); // ������ �����
			drawString(GLUT_BITMAP_HELVETICA_18, text, 0.01, 0.95);
			glPopMatrix();
		}
		
		else if(finish) {
			glOrtho(0, 1, 0, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glColor3f(0, 0, 0); // red �����
			drawString(GLUT_BITMAP_TIMES_ROMAN_24, text1, 0.1, 0.75);
			drawString(GLUT_BITMAP_TIMES_ROMAN_24, text, 0.34, 0.7);
			if (ShowScores) {
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, top, 0.43, 0.6);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, slow, 0.15, 0.55);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text5, 0.15, 0.5);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text6, 0.15, 0.45);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text7, 0.15, 0.4);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, mid, 0.45, 0.55);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text9, 0.45, 0.5);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text10, 0.45, 0.45);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text11, 0.45, 0.4);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, hard, 0.75, 0.55);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text12, 0.75, 0.5);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text13, 0.75, 0.45);
				drawString(GLUT_BITMAP_TIMES_ROMAN_24, text14, 0.75, 0.4);
				

			}

			glPopMatrix();
		}
		
		//����� ������ � �������� ��������:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(60, width / height, 1, 100);

		//����� ������ � ������� �������� 
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0, 0, distZ); // ������ ��������� �� distZ,

		glRotatef(angleX, 0.0f, 1.0f, 0.0f);  // ���������� �� �� Oy
		glRotatef(angleY, 1.0f, 0.0f, 0.0f);  // ���������� �� �� Ox
		glEnable(GL_DEPTH_TEST); //����� �������

		glEnable(GL_LIGHTING); //����� ��� ��������� ��������� 

		// ������ ������� ����� � "����":
		glEnable(GL_LIGHT0);

		//��� �������
		if (!StartScreen and !NameEnter) {
			if (!finish) {
				for (auto el : shapes) {
					el->draw();
				}
				TurnSnake(dir);
				snake->draw();
				if (foods.size() != 0) {
					for (int i = 0; i < foods.size(); i++)
					{
						foods[i]->draw();
					}
				}
			}
		}
		else {
			for (int i = 0; i < btns.size(); i++)
			{
				btns[i]->draw();
			}
		}
		// �������� ���, �� ��������:
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glFlush();
		// �������� ����:
		glutSwapBuffers(); // ����������� ������

	} 
	void on_special(int key, int x, int y) {
		switch (key) {
		case GLUT_KEY_F1:
			pause ? pause = false : pause = true;
			break;
		case GLUT_KEY_F2:   // ������ ����
			initialize();
			if (shapes.size() > 1 and !Walls) {
				shapes.pop_back(); shapes.pop_back();
				shapes.pop_back(); shapes.pop_back();
			}
			break;
		case GLUT_KEY_F3:
			NameEnter = true; StartScreen = false;
			sprintf(text8, "                                                   ");
			sprintf(text5, "                                                   ");
			sprintf(text6, "                                                   ");
			sprintf(text7, "                                                   ");
			sprintf(text9, "                                                   ");
			sprintf(text10, "                                                  ");
			sprintf(text11, "                                                  ");
			sprintf(text12, "                                                  ");
			sprintf(text13, "                                                  ");
			sprintf(text14, "                                                  ");
			sprintf(nm, "                                                   ");
			name.clear(); scores.clear(); scores1.clear(); scores2.clear();
			Walls = true;
			finish = false;
			hard_mode = 10;
			BP1 = true, BP2 = false, BP3 = false;
			for (int i = 0; i < btns.size(); i++)
			{
				delete btns[i];
			}
			btns[0] = new Board(12.5, 12, 0.0, 1, 2, 2, diffYellow, ambiYellow, specYellow);
			btns[1] = new Board(12.5, 12, 3.5, 0.1, 2, 2, diffRed, ambiRed, specRed);
			btns[2] = new Board(12.5, 12, 7, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
			btns[3] = new Board(12.5, 12, 10.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
			break;
		case GLUT_KEY_F4:
			ShowScores ? ShowScores = false : ShowScores = true;
			break;
		case GLUT_KEY_F7:
			ClearScore();
			break;
		case GLUT_KEY_DOWN:
			if(!IsPause())
				setdir(DOWN);
			else
				MoveCoin(DOWN);
			break;
		case GLUT_KEY_UP:
			if (!IsPause())
				setdir(UP);
			else
				MoveCoin(UP);
			break;
		case GLUT_KEY_LEFT:
			if (!IsPause())
				setdir(LEFT);
			else
				MoveCoin(LEFT);
			break;
		case GLUT_KEY_RIGHT:
			if (!IsPause())
				setdir(RIGHT);
			else
				MoveCoin(RIGHT);
			break;
		}
		
	}
	void on_motion(int x, int y) { //���������� �������
		if (button == 2) {
			angleX += x - mouseX;
			angleY += y - mouseY;
			mouseX = x;
			mouseY = y;
		}
	}
	void on_mouse(int button, int state, int x, int y) {
		mouseX = x;
		mouseY = y;
		this->button = button;
		
		if (button == 3) { //������ ����
			distZ += 1;
		}
		else if (button == 4) {
			distZ -= 1;
		}
		else if (button == 0) {
			count++;
			if (count < 2) {
				if ((mouseX >= 197 and mouseX <= 243) and (mouseY >= 280 and mouseY <= 320)) { //first button
					if (Walls) {
						delete btns[0];
						btns[0] = new Board(12.5, 12, 0.0, 1, 2, 2, diffRed, ambiRed, specRed);
						Walls = false;
					}
					else {
						delete btns[0];
						btns[0] = new Board(12.5, 12, 0.0, 1, 2, 2, diffYellow, ambiYellow, specYellow);
						Walls = true;
					}
				}
				else if (!BP1 and mouseX >= 200 and mouseX <= 242 and mouseY >= 351 and mouseY <= 391) { //second button
					delete btns[1], btns[2], btns[3];
					btns[1] = new Board(12.5, 12, 3.5, 0.1, 2, 2, diffRed, ambiRed, specRed);
					hard_mode = EASY; BP1 = true; BP2 = false; BP3 = false;
					btns[2] = new  Board(12.5, 12, 7, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
					btns[3] = new Board(12.5, 12, 10.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
				}
				else if (!BP2 and mouseX >= 199 and mouseX <= 234 and mouseY >= 423 and mouseY <= 465) { //third button
					delete btns[1], btns[2], btns[3];
					hard_mode = MEDIUM; BP1 = false; BP2 = true; BP3 = false;
					btns[1] = new Board(12.5, 12, 3.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
					btns[2] = new  Board(12.5, 12, 7, 0.1, 2, 2, diffRed, ambiRed, specRed);
					btns[3] = new Board(12.5, 12, 10.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
				}
				else if (!BP3 and mouseX >= 200 and mouseX <= 243 and mouseY >= 498 and mouseY <= 541) { //fourth button
					delete btns[1], btns[2], btns[3];
					hard_mode = HARD; BP1 = false; BP2 = false; BP3 = true;
					btns[1] = new Board(12.5, 12, 3.5, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
					btns[2] = new  Board(12.5, 12, 7, 0.1, 2, 2, diffYellow, ambiYellow, specYellow);
					btns[3] = new Board(12.5, 12, 10.5, 0.1, 2, 2, diffRed, ambiRed, specRed);
				}
			}
			else if (count >= 2)
				count = 0;
		}
	}
	void on_timer(int value) {
		tick++;
		if (!StartScreen and !NameEnter) {
			if (!pause) {
				if (tick >= hard_mode) { //�������� ������
					if (!finish)
						move();
					tick = 0;
				}
			}
		}
		on_paint();
	}
};

Scene* scene;

void on_motion(int x, int y)
{
	// ��������� �������� ������� ����� Scene:
	scene->on_motion(x, y);
}

void on_mouse(int button, int state, int x, int y)
{
	// ��������� �������� ������� ����� Scene:
	scene->on_mouse(button, state, x, y);
}

void on_size(int width, int height)
{
	// ��������� �������� ������� ����� Scene:
	scene->on_size(width, height);
}

void on_keyboard(unsigned char key, int x, int y)
{
	// ������� ���� �� ���������:
	if (scene->GetState()) {
		if (key == 13)
			scene->SetState(false);
		else if (key == 27)
			exit(0);
		else if (key == 8 or key == 127)
			scene->DelName();
		else
			scene->SetName(key);
	}
	else {
		if (!scene->IsPause()) {
			switch (key) {
			case 27:
				exit(0);
				break;
			case 'a':
				scene->setdir(LEFT);
				break;
			case 'd':
				scene->setdir(RIGHT);
				break;
			case 'w':
				scene->setdir(UP);
				break;
			case 's':
				scene->setdir(DOWN);
				break;
			}
		}
	}
}

void on_special(int key, int x, int y)
{
	scene->on_special(key, x, y);
}

void on_paint()
{
	// ��������� �������� ������� ����� Scene:
	scene->on_paint();
}

void on_timer(int value)
{
	// ������� ��䳿 �� �������
	scene->on_timer(value);
	glutTimerFunc(25, on_timer, 0); // �a 25 �� ����������� �� �������
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	glutInit(&argc, argv);         // ���������� GLUT
	scene = new Scene();   // ��������� ��'��� "�����"
	glutInitWindowSize(800, 600);  // ������������ ������ ����
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);// ���������� ������ �����������
	glutCreateWindow("Snake");     // ��������� ����
	glutDisplayFunc(on_paint);     // �������� ������� �����������
	glutReshapeFunc(on_size);  // �������� ������� ������� ���� ������ ����
	glutMotionFunc(on_motion);     // �������� �������, ��� ������� �� ���������� ���� � ���������� �������
	glutMouseFunc(on_mouse);
	glutKeyboardFunc(on_keyboard); // �������� �������, ��� ������� �� ���������� �����
	glutSpecialFunc(on_special);// �������� �������, ��� ������� �� ���������� ����������� �����
	glutTimerFunc(25, on_timer, 0);
	glutMainLoop();                // �������� �������� ���� ������� ����
	delete scene;                  // ��������� ��'��� "�����"
	return(0);
}

