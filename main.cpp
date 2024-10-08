#include <iostream>
#include <string>
#include "HttpClient.h"
#include "LuckOfTheDrawGame.h"
#include "GuessingGame.h"
#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////    GAME OVERVIEW   ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//	//DECKOFCARDS FINAL PROJECT - JUSTIN SMITH, NGUYEN TRAN, VIOLET YOUSIF 
//
//	//This project builds off of the work we have been doing in ITCS 2550
//		//for weeks, and uses the HttpClient class to connect to the
//		//internet and both send requests and read from an API called 
//		//Deck of Cards. Using this API, we send in a request for a user
//		//requested number of cards, get dealt back that many cards out of 
//		//a deck of 52/54, and then have the user play a little mini game 
//		//using these randomly dealt cards. 
//
//	//There are two mini games- GuessingGame (Easy Mode) and 
//		//LuckOfTheDrawGame (Hard Mode). Using these two games within the
//		//project, we use multiple different concepts covered within the 
//		//semester. Inheritence, Polymorphism, Overloaded Operators, 
//		//Templates, Enums, Vectors, Searching, Sorting, Exception Handling, 
//		//Dynamic Memory Allocation, Double Pointers, Maps, and a Clean UI are all
//		//utilized at one point or another throughout this program. 
//
//	//In Minigame 1, aka Easy Mode or GuessingGame, the user has to guess
//		//the value of a randomly drawn card. They have five chances to get
//		//it, and the values range from "1" for any Ace drawn to "13" for a 
//		//King
//
//	//In Minigame 2, aka Hard Mode or LuckOfTheDrawGame, the user is asked 
//		//how many cards they would like to draw, from 2-5. A deck of cards 
//		//contains 54 cards instead of the standard 52 in this game, 
//		//as two jokers are mixed into the bunch. The Minigame class then 
//		//searches for any Joker in the drawn cards and replaces it if found, 
//		//otherwise outputs a 'No Jokers Found' message. Then the user has
//		//to guess the total value of all of the cards added together. This
//		//ranges from  2 (two Aces drawn) to 64 (4 Kings and a Queen 
//		//drawn). The range is dynamic based on user input and card distribution.
//
//		//After either succeeding in guessing the total value with 5 
//		//attempts or not, the game concludes by outputting 3 blocks 
//		//of text:
//			//1. All the cards the user asked for as they were originally 
//				//returned from the API
//			//2. Those same cards, but now with the Joker(s) replaced if 
//				//any were found
//			//3. All the newly updated cards, ordered by the criteria
//				//requested by the user
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  Before reaching the classes and main, we have our global objects that can be called from anywhere. this includes the intro and exit messages
//	to the program and the value chart, which is displayed at each new game played

void GreetMessage();
void GetValueChart();
void ExitMessage();

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////    MAIN() FUNCTION   ///////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Intro message
	GreetMessage();

	bool exitProgram = false;
	//this loop ensures that a user can keep playing the minigames until they decide to quit 
	while (!exitProgram)
	{
		bool pickGame = true;
		string gameChoice;
		//this loop ensures a user inputs either a "1" or a "2" to select a game in an accepted mannor
		while (pickGame)
		{
			cout << "Would You Like to Play Easy Mode [Enter 1] or Hard Mode [Enter 2]: ";
			getline(cin, gameChoice);
			if (gameChoice == "1" || gameChoice == "2")
				pickGame = false;
			else
				cout << "\n" << "Please Input \"1\" or \"2\"." << "\n\n";
		}

		// Here we will add the code connecting main to Minigame1 (easy mode; guessing game)
		if (gameChoice == "1")
		{
			// Num/Cards defaulted to 1 for Guessing Game. Changes when user is prompted to declare number of cards to draw for Luck Of The Draw Game.

			GuessingGame g1;
			g1.Connect("deckofcardsapi.com");
			
			// Sends in the parameters to the API call for number of cards (defaulted as 1 for this game) and disables the API from bringing in jokers.
			g1.Get("/api/deck/new/draw", { {"count", to_string(1)}, {"jokers_enabled", "false"} });

			// Prints rules of game and level of difficulty
			g1.GetRules();

			// Function to loop over gameplay guesses for player.
			// The player has 5 guess attempts and is given hints of too high or too low.
			g1.RunGame();

			// Prints the drawn cards info
			g1.PrintResults();

		}

		// This is where Minigame 2 (hard mode; Luck of the Draw game) is triggered instead
		else
		{
			bool continueGetNumCards = true;
			string cinNumCardsString;

			// This block ensures, using exception handling, that a user enters the proper value (similar to the function GetPlayerGuess())
			while (continueGetNumCards)
			{
				cout << "How Many Cards Would You Like to See? [Enter Number 2-5]: ";
				try
				{
					getline(cin, cinNumCardsString);
					int incNumCards = stoi(cinNumCardsString);

					if (incNumCards < 2 || incNumCards > 5 || cinNumCardsString.find(".") != string::npos)
					{
						throw InvalidEntry();
					}
                    
					continueGetNumCards = false;
				}
				catch (const InvalidEntry&) {
					cout << "\n" << "Invalid Entry. Please Enter an Integer in the Given Range." << "\n\n";
				}
				catch (invalid_argument&) {
					cout << "\n" << "Invalid Entry. Please Enter an Integer in the Given Range." << "\n\n";
				}
			}

			cout << "\n";

			//here we take the user input for number of cards to be drawn, convert it to an int, 
				//add 2 to account for the bonus cards used to replace Jokers if need be, and place that number
				//into cinNumCardsString. This string is what is sent over to the API
			uint16_t numCardsDealt = static_cast<uint16_t>(stoi(cinNumCardsString)) + 2;

			LuckOfTheDrawGame g2;
			g2.Connect("deckofcardsapi.com");

			//This connects to Get(), which is an overiden function from HttpClient. Get() Calls the original function first,
			//which then calls Data and EndOfData in Deck of Cards, as LOTDG inherits 
			//publically from both classes. Then, we set its private variable 'numCardsRequested' to be equal to cardsDealt, 
			// which was calculated in EndOfData()
			// We set jokers to true so that we can implement the joker search function.
			g2.Get("/api/deck/new/draw", { {"count", to_string(numCardsDealt)}, {"jokers_enabled", "true"} });

			// Prints rules of game and level of difficulty
			g2.GetRules();

			//then we search for and potentially replace the jokers
			g2.SearchForJoker();

			//Gets and saves the correct answer, runs the guessing game, and has user pick sorting method
			g2.RunGame();

			//Finally, all versions of the cards are output to the user 
			g2.PrintResults();
		}

		//This final block just asks the user if they want to play again, ending if they enter an "x", and continuing otherwise
		// If they do, they're looped back and given the option again to choose the level of difficulty
		cout << "\n" << "Do You Wish to Exit Program? [Enter \"x\" for \'Yes\', Any Other Key for \'No\']: ";
		string exit;
		getline(cin, exit);

		if (exit == "x")
			exitProgram = true;
		else {
			system("cls");
			GetValueChart();
		}

	}

	// Exit message displayed
	ExitMessage();
	return 0;
};

////////////////////////////////////////////////////////////////////////
//////////////////////    HELPER FUNCTIONS   ///////////////////////////
////////////////////////////////////////////////////////////////////////


void GreetMessage()
{
	// prints the heart, spade, club, and diamond extended ASCII characters
	string cardSymbols = "\x03  \x04  \x05  \x06  ";
	string greeting = "   Welcome To The DECK OF CARDS Minigame!    ";
	cout << setfill('*') << setw(70) << " " << "\n";
	cout << setfill('_') << setw(70) << " " << "\n";
	cout << "\n" << " " << cardSymbols << greeting << cardSymbols << "\n";
	cout << setw(70) << setfill('_') << " " << "\n" << "\n" << "\n";
	cout << setfill(' ');

	// Print Chart for face (number) values to show player rules of guessing range
	// Print Chart for suit values to show player how suits will be sorted if chosen in sort and if used in compareXXX against numbers that are the same
	GetValueChart();

}



// Print Chart for face (number) values and suit values to show player rules of guessing card range
void GetValueChart()
{
	// Using arrays to print charts because using the global map<int,vector<string>> would cause instant reordering due to its nature as a container
	string tempNumVect[13] = { "ACE","2","3","4","5","6","7","8","9","10","JACK","QUEEN","KING" };
	string tempSuitVect[4] = { "Spades","Clubs","Diamonds","Hearts" };

	// Print Chart for Number Values
	cout << setfill('*') << "" << right << setw(46) << " ORDER FOR NUMBER VALUE CHART: " << setfill('*') << setw(17) << "" << "\n" << "\n";
	for (int i = 0; i < 13; i++)
		cout << setfill(' ') << setw(30) << right << tempNumVect[i] << " : " << i + 1 << "\n";
	cout << "\n";

	// Print Chart for Suit Values
	cout << setfill('*') << "" << right << setw(46) << " ORDER FOR SUIT VALUE CHART: " << setfill('*') << setw(18) << "" << "\n" << "\n";
	for (int i = 0; i < 4; i++)
		cout << setfill(' ') << setw(30) << right << tempSuitVect[i] << " : " << i + 1 << "\n";
	cout << endl;
}



void ExitMessage()
{
	string cardSymbols = "\x03  \x04  \x05  \x06  ";		// prints the heart, spade, club, and diamond extended ASCII characters
	string exitMessage = "  GAME TERMINATED: Thank you for playing!    ";
	cout << "\n" << "\n" << setfill('_') << setw(70) << right << " " << "\n";
	cout << "\n" << " " << cardSymbols << exitMessage << cardSymbols << "\n";
	cout << setw(70) << setfill('_') << " " << "\n" << "\n";
	cout << setw(70) << setfill('*') << " " << "\n" << endl;
}
