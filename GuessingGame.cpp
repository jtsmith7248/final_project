// This file holds the GuessingGame child class of DeckOfCards
// A card is drawn at random, the user has a limited number of attempts to guess it right

class GuessingGame : public DeckOfCards
{
private:
	int userGuess;		// retrieves number of user guesses

public:

	// MAYBE: void DrawHand(DrawCard*, int)
	DrawCard* DrawHand(int count) {
		// Draw card at index 1 (default value)
	}

	int GetGuess(string res)
	{
		// convert string to int value
		// assign conversion to userGuess
	}

	void CheckGuess(int guess)
	{
		// Get users guess by int value
		// loop using max value
		// do-while(guess < 5) function
			// if guess != answer
				// if guess > answer
				// else if guess < answer
				// else throw "invalid entry"
			// else if guess == answer
	}

	string SendResponse()
	{
		// use CheckGuess to determine response
		// Maybe move do-while in here and if statements in CheckGuess
		// Response: if user won
		// Response: if user lost
	}
}