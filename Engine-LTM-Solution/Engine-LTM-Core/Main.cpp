#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#pragma region MACROES, ASSERTIONS, STATIC ASSERTIONS
void f() {
#ifdef _DEBUG
	printf("Calling f in debug mode\n");
#else
	printf("Calling f in nondebug mode\n");
#endif
}

void log_debug() {
	printf("Log debug!!!\n");
}

void log_nondebug() {
	printf("Log nondebug!!!\n");
}

#ifdef USER_DEBUG
#define _LogInGeneral() log_debug()
#else
#define _LogInGeneral() log_nondebug()
#endif

#if ASSERTIONS_ENABLED
// define some inline assembly that causes a break
// into the debugger -- this will be different on each
// target CPU
//#define debugBreak() __asm { int 3 }
// check the expression and fail if it is false
#define ASSERT(expr) \
if (expr) {\
	printf("The expression is true\n");\
} \
else \
{ \
	printf("The expression is false\n");\
	__debugbreak(); \
}
#else
#define ASSERT(expr) // evaluates to nothing
#endif

struct NeedsToBe128Bytes
{
	uint32_t m_a;
	uint32_t m_b;
	uint32_t m_c;
	uint32_t m_d;
	uint32_t m_e;
	uint32_t m_f;
	uint32_t m_g;
	uint32_t m_h;
	uint32_t m_i;
	uint32_t m_k;
	uint32_t m_l;
	uint32_t m_m;
	uint32_t m_n;
	uint32_t m_o;
	uint32_t m_p;
	uint32_t m_q;
	uint32_t m_r;
	uint32_t m_s;
	uint32_t m_t;
	uint32_t m_u;
	uint32_t m_v;
	uint32_t m_w;
	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_z;
	uint32_t m_1;
	uint32_t m_2;
	uint32_t m_3;
	uint32_t m_5;
	uint32_t m_6;
	uint32_t m_7;
	uint32_t m_8;
	// etc.
};

#define _ASSERT_GLUE(a, b) a ## b
#define ASSERT_GLUE(a, b) _ASSERT_GLUE(a, b)
#define STATIC_ASSERT(expr) \
	enum \
	{ \
		ASSERT_GLUE(g_assert_fail_, __LINE__) \
		= 1 / (int)(!!(expr)) \
	}

#pragma endregion
#pragma region INHERITANCE, VTABLE TEST

class Shape {
public:
	virtual void Draw() {
		printf("Do drawing!\n");
	}
};

class Animator {
public:
	virtual void Animate() {
		printf("Do animation!\n");
	}
};

class Circle : public Shape, public Animator
{
public:
	virtual void Draw() override {
		printf("Do circle drawing!\n");
	}

	virtual void Animate() override {
		printf("Do circular animation!\n");
	}
};

class Rectangle : public Shape, public Animator
{
public:
	virtual void Draw() override {
		printf("Do rectangle drawing!\n");
	}

	virtual void Animate() override {
		printf("Do rectangle animation!\n");
	}
};

class Triangle : public Shape, public Animator
{
public:
	virtual void Draw() override {
		printf("Do Triangle drawing!\n");
	}

	virtual void Animate() override {
		printf("Do Triangle animation!\n");
	}
};
#pragma endregion
#pragma region ABSTRACT FACTORY, PROTOTYPE TEST
static int curr_loc = 0;

class MapSite {
public:
	virtual void Enter() = 0;
	virtual std::string ToString() = 0;
};

enum Direction {
	North,
	South,
	East,
	West
};

class Room : public MapSite {
public:
	Room()
	{
		_roomNumber = count++;
	}

	Room(const Room& other)
		: Room()
	{
		for (int i = 0; i < 4; i++)
			_sides[i] = other._sides[i];
	}

	virtual std::shared_ptr<Room> Clone() const {
		return std::make_shared<Room>(*this);
	}

	virtual std::string ToString() {
		return "Room No: " + std::to_string(_roomNumber) + "\nSides: "
			+ _sides[0]->ToString() + "\n"
			+ _sides[1]->ToString() + "\n"
			+ _sides[2]->ToString() + "\n"
			+ _sides[3]->ToString();
	}

	std::shared_ptr<MapSite> GetSide(Direction d) const {
		return _sides[d];
	}

	void SetSide(Direction d, std::shared_ptr<MapSite> s) {
		_sides[d] = s;
	}

	virtual void Enter() {
		curr_loc = _roomNumber;
	}

	int GetRoomNumber() const {
		return _roomNumber;
	}
private:
	std::shared_ptr<MapSite> _sides[4];
	static int count;
	int _roomNumber;
};

int Room::count = 0;

class EnchantedRoom : public Room
{
public:
	EnchantedRoom(bool has_spell)
		: _hasSpell(has_spell)
	{

	}

	EnchantedRoom(const EnchantedRoom& other)
		: Room(other)
	{
		_hasSpell = other._hasSpell;
	}

	virtual std::string ToString() {
		return Room::ToString() + ";Has Spell: " + std::to_string(_hasSpell);
	}

	bool HasSpell() const {
		return _hasSpell;
	}

	virtual std::shared_ptr<Room> Clone() const {
		return std::make_shared<EnchantedRoom>(*this);
	}
private:
	bool _hasSpell;
};

class Wall : public MapSite {
public:
	Wall() {};

	Wall(const Wall& other)
	{

	}

	virtual std::shared_ptr<Wall> Clone() {
		return std::make_shared<Wall>(*this);
	}

	virtual std::string ToString() {
		return "There's a wall!";
	}

	virtual void Enter() {
		printf("You hit a wall, fellow explorer!!!");
	}
};

class BombedWall : public Wall
{
public:
	BombedWall(bool has_bomb)
		: _hasBomb(has_bomb)
	{

	}

	BombedWall(const BombedWall& other)
		: Wall(other)
	{

	}

	virtual std::string ToString() {
		return Wall::ToString() + ";Wall has bomb: " + std::to_string(_hasBomb);
	}

	virtual std::shared_ptr<Wall> Clone() {
		return std::make_shared<BombedWall>(*this);
	}

	bool HasBomb() const { return _hasBomb; }
private:
	bool _hasBomb;
};

class Door : public MapSite {
public:
	Door(std::shared_ptr<Room> room1 = 0, std::shared_ptr<Room> room2 = 0, bool isOpen = false)
		: _room1(room1), _room2(room2), _isOpen(isOpen)
	{

	}

	Door(const Door& other)
	{
		_room1 = other._room1;
		_room2 = other._room2;
		_isOpen = other._isOpen;
	}

	virtual std::shared_ptr<Door> Clone() {
		return std::make_shared<Door>(*this);
	}

	virtual std::string ToString() {
		std::string str = "";
		if (_room1)
			str += "Room1: " + std::to_string(_room1->GetRoomNumber()) + ";";
		if (_room2)
			str += "Room2: " + std::to_string(_room2->GetRoomNumber()) + ";";
		if (isalnum(_isOpen))
			str += "Is Open: " + std::to_string(_isOpen) + ";";
		return str;	
	}

	virtual void Initialize(std::shared_ptr<Room> r1, std::shared_ptr<Room> r2, bool _isOpen) {
		_room1 = r1;
		_room2 = r2;
		this->_isOpen = _isOpen;

	}

	virtual void Enter() {
		if (_isOpen)
			if (curr_loc == _room1->GetRoomNumber())
				curr_loc = _room2->GetRoomNumber();
			else
				curr_loc = _room1->GetRoomNumber();
		else
			printf("You hit the door!!!");
	}

	std::shared_ptr<Room> OtherSideFrom(std::shared_ptr<Room> r) {
		if (r == _room1)
			return _room2;
		return _room1;
	}
private:
	std::shared_ptr<Room> _room1;
	std::shared_ptr<Room> _room2;
	bool _isOpen;

};

class Bomb
{
public:
	Bomb() {}
	Bomb(const Bomb& other) {

	}
	std::shared_ptr<Bomb> Clone() {
		std::shared_ptr<Bomb> b = std::make_shared<Bomb>();
		return b;
	}
	virtual std::string ToString() {
		return "Here's a bomb!!";
	}
	void Explode() { printf("BOOMMMM!!!"); }
};

class BombedDoor : public Door {
public:
	BombedDoor(std::shared_ptr<Room> r1, std::shared_ptr<Room> r2, bool isOpen, bool has_bomb, Bomb* b)
		: Door(r1, r2, isOpen), _hasBomb(has_bomb), _bomb(b)
	{

	}

	BombedDoor(const BombedDoor& other)
		: Door(other), _hasBomb(other._hasBomb), _bomb(other._bomb)
	{

	}

	bool HasBomb() const {
		return _hasBomb;
	}

	void SetRoom1(std::shared_ptr<Room> r1) {
		r1 = r1;
	}

	void SetRoom2(std::shared_ptr<Room> r2) {
		r2 = r2;
	}

	std::shared_ptr<Door> Clone() const {
		return std::make_shared<BombedDoor>(*this);
	}

	virtual std::string ToString() {
		return Door::ToString() + ";Has Bomb: " + std::to_string(_hasBomb) + ";" + _bomb->ToString();
	}
private:
	bool _hasBomb;
	std::shared_ptr<Bomb> _bomb;
};

class Maze {
public:
	Maze() {

	}

	Maze(const Maze& other) {
		for (auto r : other.room_collection)
			room_collection.push_back(r->Clone());

	}

	virtual std::shared_ptr<Maze> Clone() {
		return std::make_shared<Maze>(*this);
	}

	virtual std::string ToString()
	{
		if (!room_collection.empty()) {
			std::string str = room_collection.at(0)->ToString();
			for (int i = 1; i < room_collection.size(); i++)
				str += "\n" + room_collection.at(i)->ToString();
			return str;
		}
		return "";
	}

	void AddRoom(std::shared_ptr<Room> r) {
		room_collection.push_back(r);
	}

	std::shared_ptr<Room> RoomNo(int rNo) {
		return *std::find_if(room_collection.begin(), room_collection.end(), [&](std::shared_ptr<Room> r) {return r->GetRoomNumber() == rNo; });
	}
private:
	std::vector<std::shared_ptr<Room>> room_collection;
};

class MazeFactory {
public:
	MazeFactory(std::shared_ptr<Maze> maze, std::shared_ptr<Wall> wall, std::shared_ptr<Room> room, std::shared_ptr<Door> door)
		: maze(maze), wall(wall), room(room), door(door) {};

	virtual std::shared_ptr<Maze> MakeMaze() const { return maze->Clone(); }
	virtual std::shared_ptr<Wall> MakeWall() const { return wall->Clone(); }
	virtual std::shared_ptr<Room> MakeRoom() const { return room->Clone(); }
	virtual std::shared_ptr<Door> MakeDoor(std::shared_ptr<Room> r1 = 0, std::shared_ptr<Room> r2 = 0, bool _isopen = false) const {
		auto clone = door->Clone();
		clone->Initialize(r1, r2, _isopen);
		return clone;
	}
private:
	std::shared_ptr<Maze> maze;
	std::shared_ptr<Wall> wall;
	std::shared_ptr<Room> room;
	std::shared_ptr<Door> door;
};

class EnchantedFactory : public MazeFactory
{
public:
	EnchantedFactory(std::shared_ptr<Maze> maze, std::shared_ptr<EnchantedRoom> eroom, std::shared_ptr<Wall> wall, std::shared_ptr<Room> room, std::shared_ptr<Door> door)
		: MazeFactory(maze, wall, eroom, door)
	{

	}
};


class MazeGame {
public:
	static std::shared_ptr<Maze> CreateMaze(MazeFactory* factory) {
		std::shared_ptr<Maze> aMaze = factory->MakeMaze();
		std::shared_ptr<Room> r1 = factory->MakeRoom();
		std::shared_ptr<Room> r2 = factory->MakeRoom();
		std::shared_ptr<Door> theDoor = factory->MakeDoor(r1, r2, true);

		aMaze->AddRoom(r1);
		aMaze->AddRoom(r2);

		r1->SetSide(North, factory->MakeWall());
		r1->SetSide(East, theDoor);
		r1->SetSide(South, factory->MakeWall());
		r1->SetSide(West, factory->MakeWall());

		r2->SetSide(North, factory->MakeWall());
		r2->SetSide(East, factory->MakeWall());
		r2->SetSide(South, factory->MakeWall());
		r2->SetSide(West, factory->MakeDoor());

		return aMaze;
	}
};
#pragma endregion

int main() {

	/*f();
	_LogInGeneral();
	ASSERT(5 != 5);
	STATIC_ASSERT(sizeof(NeedsToBe128Bytes) == 128, "Wrong size");*/

	auto m = std::make_shared<Maze>();
	auto w = std::make_shared<Wall>();
	auto r = std::make_shared<Room>();
	auto bw = std::make_shared<BombedWall>(true);
	auto er = std::make_shared<EnchantedRoom>(true);
	auto d = std::make_shared<Door>();

	MazeFactory f(m, bw, r, d);
	EnchantedFactory ef(m, er, w, r, d);

	auto maze = MazeGame::CreateMaze(&ef);

	printf(maze->ToString().c_str());

	system("pause");
}