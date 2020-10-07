#include "cmpt_error.h"    // These are the only permitted includes!
#include "Chatbot.h"       //
#include <iostream>        // Don't include anything else.
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cassert>

using namespace std;

int random_number(vector<string> input);

class User_bot : public Chatbot{
private:

	string user_name;
	string user_reply;

public:

	User_bot(string name)
	: user_name(name)
	{ }

	string name() const{
		return user_name;
	}

	string get_reply(){		
		cout << "\n\nEnter line of text" << "\n";	
		getline(cin,user_reply);
		return user_reply;	
	}	
	
	void tell(const string& ){
		// cout << s;
	}
};

// *************************** SMART BOT OVERVIEW ***************************
// HOW TO USE.
// 1. In the main just include Smart_bot sb("SurveyBot");
// 2. With converse as well my Smart_bot should be first in the parameters like this converse(&sb,&user);
// 3. It will then pop up the opening message and follow what it ask to input.
// ALSO PUT THE TERMINAL WINDOW IN FULL SCREEN TO SEE ALL TEXT PROPERLY

// ADDITIONAL NOTES.
// Each else if is each step of the survey by order when the program is ran. Once all survey questions are answered.
// done_survey will be true and will finish the ending parts of the program. I did not include a stop whole program
// Because usually chatbots on facebook messenger can just be closed to be finished. 
// So just ending the program by ctrl-c is technically stopping the program.
class Smart_bot : public Chatbot{
private:

	string bot_name;
	string user_reply;
	string answer;
	const string opening_msg = "\n\nHi I'm SurveyBot where I'm used in restaurants to see the feedback of their customers."
							   "\nI will ask you four questions related to your experience today."
						       "\n\nFirst off, what is your name?";
	const string scale_msg = "\n\n1. On a scale of 1 - 10, how satisfied were you with the service?"
					         "\n   (Terrible = 1 & Excellent = 10)\n"
					         "   (Please enter a whole number between 1 - 10 for your answer)";					 
	string user_name;
	string rate_number;
	string rate_others;
	string rate_quality;
	string comment_given;

	bool program_start = false;
	bool given_name = false;
	bool given_number = false;
	bool given_others = false;
	bool given_quality = false;
	bool review_answers = false;
	bool done_survey = false;
	bool done_steps = false;

public:	

	Smart_bot(string given_name)
	: bot_name(given_name)
	{ }

	string name() const {
		return bot_name; 
	}

	void tell(const string& s){
		string change = s;		

		for(int i = 0; i < change.size(); i++){
			if(change[i] >= 97){
				replace(change.begin(),change.end(),change[i],char(((int)change[i])-32));
			}
		}
		user_reply = change;
	}		

	string get_reply(){
			
		if(!program_start){
			answer = opening_msg;
			program_start = true;
		}else{
			if(check_name(user_reply) && !given_name){		
				given_name = true;
				user_name = user_reply;	
				answer = "\n\nHello " + user_name + "! \nThank you for taking the time to take this survey, now let's get started..." + scale_msg;	
			}else if(!check_name(user_reply) && !given_name){		
				answer = "\n\nSorry I only accept letters as a given name\nWhat Is Your Name?"; 			
			}else if(check_number(user_reply) && given_name && !given_number){
				given_number = true;
				rate_number = user_reply;
				answer = bot_response(user_reply) + "\n\n2. Would you recommend this restaurant to friends and family?\n   Yes | No | Maybe";
			}else if(!check_number(user_reply) && given_name && !given_number){
				answer = "\n\n" + wrong_response() + "\nPlease enter a number between 1 - 10";
			}else if(check_answer(user_reply) && given_name && given_number && !given_others){
				given_others = true;
				rate_others = user_reply;
				answer = bot_response(user_reply) + "\n\n3. How did you find the quality/taste of the food?\n   Horrible | Average | Delicious";
			}else if(!check_answer(user_reply) && given_name && given_number && !given_others){
				answer = "\n\n" + wrong_response() + "\nPlease answer either Yes, No, or Maybe";					
			}else if(check_taste(user_reply) && given_name && given_number && given_others && !given_quality){
				given_quality = true;
				rate_quality = user_reply;
				answer = bot_response(user_reply) + "\n\n4. Any additional comments? \n   Please type anything else you would like to say below... \n   (If no comments just hit ENTER)";
			}else if(!check_taste(user_reply) && given_name && given_number && given_others && !given_quality){
				answer = "\n\n" + wrong_response() + "\nPlease answer either Horrible, Average, or Delicious";
			}else if(check_comment(user_reply) && given_name && given_number && given_others && given_quality && !done_steps){
				done_steps = true;
				comment_given = user_reply;
			}		
		}
		
		if(done_steps && !review_answers){
			review_answers = true;
			answer = "\n\nIs this all correct to submit " + user_name + "?\n" + review(rate_number,rate_others,rate_quality);
		}else if(check_confirm(user_reply) && done_steps && review_answers && !done_survey){
			if(user_reply.find("SUBMIT") != string::npos){
				done_survey = true;	
				answer = thankyou_msg() + "\nEnter AGAIN if wanting to do the survey again";
			}
			if(user_reply.find("NO") != string::npos){

				given_name = false;
				given_number = false;
				given_others = false;
				given_quality = false;
				done_steps = false;
				review_answers = false;
				done_survey = false;
				answer = opening_msg;
			}
		}else if((user_reply.find("AGAIN") != string::npos) && done_survey){

				given_name = false;
				given_number = false;
				given_others = false;
				given_quality = false;
				done_steps = false;
				review_answers = false;
				done_survey = false;
				answer = opening_msg;
		
		}else if(!check_confirm(user_reply) && done_steps && review_answers && !done_survey){	
				answer = "\n\n" + wrong_response() + "\nPlease review your answers and follow instructions below...\n" + review(rate_number,rate_others,rate_quality);
		}else if(!check_word_again(user_reply) && done_steps && review_answers && done_survey){
				answer = "\n\nPlease enter AGAIN if wanting to take the survey again";
		}

		return answer;
	}

	string bot_response(string input){

		input.erase(remove(input.begin(),input.end(),' '),input.end());
		input.erase(remove(input.begin(),input.end(),'	'),input.end());

		vector<string> negative_msg;
		vector<string> positive_msg;
		string type_response;

		negative_msg.push_back("\n\nThat's not quite the feedback we are wanting to hear, but we understand.");
		negative_msg.push_back("\n\nThat's really low for our standards, but we will try to fix that!");
		positive_msg.push_back("\n\nAwesome that's great to hear!");
		positive_msg.push_back("\n\nThat's great! Glad you felt that way!");

		type_response = negative_msg[random_number(negative_msg)];

		if(check_number(input)){
			int good_number = stoi(input);
				if(good_number >= 5 && good_number <= 10){
					type_response = positive_msg[random_number(positive_msg)];
				}
		}

		if(check_answer(input) || check_taste(input)){
			if(input == "YES" || input == "DELICIOUS"){
				type_response = positive_msg[random_number(positive_msg)];
			}

			if(input == "MAYBE" || input == "AVERAGE"){
				type_response = "\n\nThat's alright! We understand your decision.";
			}
		}

		return type_response;
	}	
	
	string user_comment(string comment_input){

		comment_input.erase(remove(comment_input.begin(),comment_input.end(),'	'),comment_input.end());
		string comment_review = "\n\nAdditional Comments...\nA: ";
			
		if(comment_input.size() == 0 && comment_input == ""){
			return "";
		}

		return comment_review + comment_input;
	}

	string wrong_response(){

		vector<string> error_msg;
		string current_error;

		error_msg.push_back("I'm sorry " + user_name + ", I did not quite get that.");
		error_msg.push_back("I believe that is not a valid answer " + user_name + ".");
		error_msg.push_back("Hm, I don't think that is one of the options " + user_name + ".");
		error_msg.push_back("Sorry " + user_name + ", that is not an option.");

		current_error = error_msg[random_number(error_msg)];

		return current_error;
	}

	string thankyou_msg(){

		vector<string> different_msg;
		string current_msg;

		different_msg.push_back("\n\nThank you " + user_name + "! for taking the time to complete this survey.\n");
		different_msg.push_back("\n\nThank you so much " + user_name + " for taking this survey, we will look over your feedback and hopefully see you back here again!\n");

		current_msg = different_msg[random_number(different_msg)];
		
		return current_msg;
	}

	string review(string input_number, string input_others, string input_quality){

		//To remove spacing and tab in input of answer	
		input_number.erase(remove(input_number.begin(),input_number.end(),' '),input_number.end());
		input_number.erase(remove(input_number.begin(),input_number.end(),'	'),input_number.end());		
		input_others.erase(remove(input_others.begin(),input_others.end(),' '),input_others.end());
		input_others.erase(remove(input_others.begin(),input_others.end(),'	'),input_others.end());
		input_quality.erase(remove(input_quality.begin(),input_quality.end(),' '),input_quality.end());
		input_quality.erase(remove(input_quality.begin(),input_quality.end(),'	'),input_quality.end());

		string info = "\nQ: On a scale of 1-10, how satisfied were you with the service?\nA: " + input_number +
					  "\n\nQ: Would you recommend this restaurant to friends and family?\nA: " + input_others +
					  "\n\nQ: How did you find the quality/taste of the food?\nA: " + input_quality + user_comment(comment_given) +
					  "\n\nEnter SUBMIT to finish or NO to restart";	

		return info;			  
	}

	bool check_number(string input){

		input.erase(remove(input.begin(),input.end(),' '),input.end());
		input.erase(remove(input.begin(),input.end(),'	'),input.end());

		int count = 0;

		for(int i = 0; i < input.size(); i++){
			if(input[i] <= 47 || input[i] >= 58){
				count++;
			}
		}

		if(input.size() == 0){
			return false;
		}

		if(input.size() > 2){
			return false;
		}	

		if(count == 0){
			int range_check = stoi(input);
				if(range_check >= 1 && range_check <= 10){
					return true;
				}
		}
	
		return false;
	}

	bool check_comment(string input){

		if(input.size() == 0){
			return true;
		}

		if(input.size() > 0){
			input.erase(remove(input.begin(),input.end(),' '),input.end());
			input.erase(remove(input.begin(),input.end(),'	'),input.end());

			return true;
		}

		return false;
	}

	bool check_answer(string input){

		if(input.find("YES") != string::npos || input.find("NO") != string::npos || input.find("MAYBE") != string::npos){
			input.erase(remove(input.begin(),input.end(),' '),input.end());
			input.erase(remove(input.begin(),input.end(),'	'),input.end());
				if(input == "YES" || input == "NO" || input == "MAYBE"){
					return true;
				}
		}
				
		return false;				
	}

	bool check_taste(string input){

		if(input.find("DELICIOUS") != string::npos || input.find("AVERAGE") != string::npos || input.find("HORRIBLE") != string::npos){
			input.erase(remove(input.begin(),input.end(),' '),input.end());
			input.erase(remove(input.begin(),input.end(),'	'),input.end());
				if(input == "DELICIOUS" || input == "AVERAGE"|| input == "HORRIBLE"){
					return true;
				}
		}
		
		return false;		
	}

	bool check_confirm(string input){

		if((input.find("SUBMIT") != string::npos) || (input.find("NO") != string::npos)){	
			input.erase(remove(input.begin(),input.end(),' '),input.end());
			input.erase(remove(input.begin(),input.end(),'	'),input.end());
				if(input == "SUBMIT" || input == "NO"){
					return true;
				}
		}		
		
		return false;
	}

	bool check_word_again(string input){

		if((input.find("AGAIN") != string::npos && input == "AGAIN")){
			input.erase(remove(input.begin(),input.end(),' '),input.end());
			input.erase(remove(input.begin(),input.end(),'	'),input.end());

			return true;

		}

		return false;
	}	
// Numbers correspond to the letters in the ASCII table. Similar to check_spell for Datetime_bot
	bool check_name(string input){
		
		input.erase(remove(input.begin(),input.end(),' '),input.end());
		input.erase(remove(input.begin(),input.end(),'	'),input.end());

		int count = 0;

		for(int i = 0; i < input.size(); i++){
			if(input[i] <= 64 || input[i] >= 91){
				count++;
			}
		}	

		if(input.size() == 0){
			return false;
		}

		if(count == 0){
			return true;
		}	

		return false;
	}	

};

int random_number(vector<string> input){

	int random_gen = std::chrono::system_clock::now().time_since_epoch().count();
	random_gen = random_gen % input.size();

	return random_gen;
}

void converse(Chatbot* a, Chatbot* b, int max_turns = 50) {
  for(int turn = 1; turn <= max_turns; turn++) {
    string a_msg = a->get_reply();
    cout << "(" << turn << ") " << a->name() << ": " << a_msg << "\n";

    turn++;
    if (turn > max_turns) return;

    b->tell(a_msg);
    string b_msg = b->get_reply();

    cout << "(" << turn << ") " << b->name() << ": " << b_msg << "\n";
    a->tell(b_msg);
  }
}

int main(){

// User Bot Test
	User_bot user{"user"};				

// Smart bot Test	
	Smart_bot sb("SurveyBot");

//Converse to test the bots	
	converse(&sb,&user);	
}