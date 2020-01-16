#include "Webgame.hpp"

class Room {
private:
  std::string _name;
  std::string _desc;
  std::map<std::string, Room *> _nextrooms;

public:
  Room(std::string name, std::string dsc) : _name(name), _desc(dsc) {}

  void addroom(Room *rm) { _nextrooms[rm->GetName()] = rm; }

  const std::string &GetName() { return _name; }

  Room *SwitchRooms(std::string name) {
    try {
      return _nextrooms.at(name);
    } catch (...) {
    }
    return nullptr;
  }

  void show(Webconsole *cout) {
    cout->write(_desc);
    cout->flush();
  }
};

Room m("main", "You see a nice room there are two exits, one on the left and "
               "one on the right\n");

Room lm("left", "This is the left room\n");
Room rm("right", "this is the right room\n");

class Game {
private:
  Webconsole *_cout;
  Room *curr;

public:
  Game(Webconsole *cout) {
    curr = &m;
    _cout = cout;
    _cout->write("Welcome to this super nice text adventure!\n");
    _cout->write(color::red, "Now some red text ", color::blue, "some blue\n");
    _cout->flush();
  }

  void onmessage(std::string kk) {
    if (kk == "show") {
      curr->show(_cout);
    } else if (kk.substr(0, 5) == "goto ") {
      std::string thisrm = kk.substr(5);
      auto ret = curr->SwitchRooms(thisrm);
      if (ret)
        ret->show(_cout);
      else {
        _cout->write(color::red, "\nThis room does not exist!\n");
        _cout->flush();
      }
    } else {
      _cout->write(color::red, "\nUnknown command!\n");
      _cout->flush();
    }
  }
};

int main() {
  m.addroom(&rm);
  m.addroom(&lm);

  rm.addroom(&m);
  lm.addroom(&m);

  int a = 1;
  int b = 2;
  a = a + b + a + b;
  Webgame<Game> gl;
  gl.run();
}
