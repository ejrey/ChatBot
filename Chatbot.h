using namespace std;

class Chatbot {
public:
  // Name of this Chatbot
  virtual string name() const = 0;

  // Give a string to this chatbot.
  virtual void tell(const string& s) = 0;

  // Get a string from this chatbot.
  virtual string get_reply() = 0;

  // virtual destructor
  virtual ~Chatbot() { }
}; // class Chatbot