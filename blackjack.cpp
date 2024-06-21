#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

// A method to calculate a random value
auto rng = default_random_engine {};

/***********************
 *  I want this project to work initially offline without worrying about tables or
 *  seats, focusing primarily on setting up the game logic. Once that has been 
 *  completed, I can optimize it and use the best coding practices.
 * 
 *  Once I have a finished product that I am happy with, I will create a new project
 *  that will attempt to augment what I have already created. For this recreation,
 *  I would like to add the ability to pick a table and seat to sit at.
 * 
 *  After that has been completed, I will then work on creating bots that use the
 *  book moves to effectively play the game. There will be instances where the book
 *  does not have a single best move, so I will need to come up with some kind of
 *  logic for the bots.
 * 
 *  Initially, I want to make a simulation of the poker game blackjack. My requirements
 *  are as follows:
 *      - The player shall have the ability to place a bet, hit, double down, split, or stand
 *      - The player shall be able to ask the dealer for the book move at any point on their turn
 *      - The dealer shall use 4 decks
 *      - The dealer shall shuffle the cards when the boot is empty or near empty
 *      - The table shall have a minimum and maximum bet size
 *      - The game should have some way to track or acknowledge the player's wins/losses
 *  
 **********************/

/**********************
 * I want to use vectors to store the cards for the deck. Since vectors can be of
 * any type, I can create a class for individual cards, and a class for decks.
 * 
 * For the cards, I will need the constructor to store the card type and value. I
 * would also like to make it easily printable, as the card values will often be
 * shown.
 * 
 * For the decks, I will need the constructor to generate a full 52 card deck. I
 * will also need a way to combine decks, as well as transfer cards from one deck
 * to another. I could also create a method that generates an empty deck, which.
 * could be used as a discard pile, or a temporary storage for moving cards around.
 * 
 **********************/

// Class used for generating an individual card card
class Card {
    public:
        int value; // 1 to 13, Ace to King
        int suit; // 1 = Clubs, 2 = Spades, 3 = Diamonds, 4 = Hearts

        // Constructor that will set the value and suit of each card
        Card (int v, int s) : value(v), suit(s) {} 

        // Allows the value and suit of a card to be printed using '<<'
        friend ostream& operator<<(ostream& os, const Card& card) {
            // Define arrays to store rank and suit strings
            const string ranks[] = { "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King" };
            const string suits[] = { "Clubs", "Spades", "Diamonds", "Hearts" };

            // Check if rank and suit values are within the valid ranges
            if (card.value < 1 || card.value > 13 || card.suit < 1 || card.suit > 4) {
                os << "Invalid card! Something went wrong.\n";
            }
            else {
                // Output the formatted card information
                os << ( card.value == 1 ? "an " : "a ") << ranks[card.value - 1] << " of " << suits[card.suit - 1];
            }

            return os;
        }
};

// Class used for generating and managing a deck of cards
class Deck {
    public:
        // Generates one of each card and stores the result in a vector
        Deck() {
            for (int i = 1; i <= 13; i++) {
                for (int j = 1; j <= 4; j++) {
                    cards.push_back(Card(i, j));
                    numCards += 1;
                }
            }
            return;
        }

        // Adds the standard 52 cards to a deck
        void fillDeck() {
            for (int i = 1; i <= 13; i++) {
                for (int j = 1; j <= 4; j++) {
                    cards.push_back(Card(i, j));
                    numCards += 1;
                }
            }

            return;
        }

        // Transfers the top card from the deck to a given vector
        void transferCard(vector<Card> &destination) {
            if (cards.empty()) {
                cout << "Deck is empty! Cannot transfer a card." << endl;
                return;
            } // Check that our deck is not empty

            destination.push_back(cards[0]);    // Transfer the first card to the destination
            cards.erase(cards.begin());         // Remove the first card from the deck

            return;
        }

        // Displays all cards within a deck
        void showCards(){
            for (int i = 0; i < numCards; i++) {
                cout << cards[i] << endl;
            }

            cout << "This deck has " << numCards << (numCards == 1 ? " card." : " cards." ) << endl;
            return;
        }

        // Combines two decks of cards
        void combine(const Deck &deck2) {
            numCards += deck2.numCards;
            cards.insert(cards.end(), deck2.cards.begin(), deck2.cards.end());

            return;
        }

        void addCards(const vector<Card> &source) {
            numCards += source.size();
            cards.insert(cards.end(), source.begin(), source.end());

            return;
        }

        // Shuffles a deck of cards, or two decks together
        void shuffle() {
            // Generate a new seed based on the current time
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();

            // Use the new seed to shuffle the cards
            std::shuffle(std::begin(cards), std::end(cards), default_random_engine(seed));

            cout << "The deck has been shuffled." << endl;
            return;
        }

    private:
        vector<Card> cards; // Stores all of the cards in the deck
        int numCards = 0;   // How many cards are within the deck
};

// Used to calculate the winnings for a player
int payout(int bet, int total, int numCards) {
    // If the player has blackjack
    if (total == 21 && numCards == 2) {
        return bet + (bet * 1.5);
    }

    // If the player wins by regular means
    return (bet * 2);
}

// Calculates the total value in a player's hand. Will return true if the player busts, otherwise false
bool total(vector<Card> hand) {
    int total1 = 0, total2 = 0;
    bool ace = false;

    for (int i = 0; i < hand.size(); i++) {
        if (hand[i].value == 1) {
            total1 += 11;
            total2 += 1;
            ace = true;
        } // If an Ace is found
        else if (hand[i].value > 10) {
            total1 += 10;
            total2 += 10;
        }  // If a face card is found
        else { 
            total1 += hand[i].value;
            total2 += hand[i].value;
        }
    }

    // Since total2 is always equal to or less than total1, we know the player is out by checking that value
    if (total2 <= 21) {
        cout << endl << "You have ";
        if (ace && (total1 == 21)) {
            cout << total1 << endl;
        }
        else if (ace && (total1 <= 21)) {
            cout << total1 << " or " << total2 << endl;
        } // If the player has an Ace
        else {
            cout << total2 << endl;
        } // Otherwise, print total

        return false;
    }
    else {
        cout << total2 << ", too many!" << endl;
        this_thread::sleep_for(chrono::seconds(1));
        return true;
    }
}

void sendCard(vector<Card>& hand, vector<Card>& destination) {
    if (!hand.empty()) {
        destination.push_back(hand[0]); // Transfer the first card to the destination
        hand.erase(hand.begin()); // Remove the first card from the deck
    } else {
        cout << "Hand is empty! Cannot transfer a card." << endl;
    }

    return;
}

void sendAllCards(vector<vector<Card>>& seats, vector<Card>& destination) {
    for (auto& seat : seats) {
        for (auto& card : seat) {
            destination.push_back(card); // Transfer card from each seat to destination
        }
    }
}

void bookMove(vector<Card> yourHand, vector<Card> dealerHand) {
    
    // Will recommend hitting or standing,
    // hit when hand is 11 or less
    // when your hand is 12-16: if the dealer's card is 7 or higher, it is recommended to hit. If 6 or lower, you may choose to stand since the dealer has a higher chance of busting
    // if you have a soft 18, its often best to stand, unless the dealer has 9, 10, or ace
    // If you have a pair of aces or 8s, it is recommended to split these pairs. if splitting isn't an option, then hit instead
    // if your total hand is 17 or lower, its generally recommended to hit unless the dealer's upcard is weak (4-6)

    // when to double down,
    // when your hand total is 9: if the dealer's card is 2-6, doubling down is recommended
    // When your hand total is 10: doubling down is recommended when the dealer has 2-9, as you have a strong chance of winning with a total of 20 or 21
    // when your hand total is 11: it's almost always recommended to double down with a total of 11, regardless of the dealer's upcard
    // when you have a soft 16-18: if the dealer's card is weak (4-6), it's recommended to double down on soft 16-18. This is because drawing a 10 won't make you bust, and you have a good chance at improving your hand

    // when to split pairs  
    // pairs of aces or 8s: always split. most casinos only allow you to draw one additional card per split ace
    // pairs of 2s, 3s, 6s, 7s: split these when the dealer's card is weak (2-7). this gives you a chance to improve your hand against a dealer who is more likely to bust
    // pairs of 9s: split 9s when the dealer's upcard is weak (2-6, 8, 9). However, when up against a 7, 10, or Ace, it is better to stand
    // pairs of 4s: split when the dealer's card is weak (5-6)
    // never split 5s or 10s!
    return;
}





/*
// Processes everything that you do on your turn
void yourTurn(Seat seat) {
    int action = -1;
    bool endTurn = false, bust = false;
    
    while (!endTurn && !bust) {
        // Perform an action on your turn
        cout << "What will you do?" << endl;
        cout << "Help [0], Hit [1], Stand [2], Double Down [3], Split [4], Count Chips [5]" << endl;

        while ((action < 0) || (action > 5) && !(cin >> action)) {
            cin >> action;

            // Clears the input buffer to prevent infinite loops
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (action < 0 || action > 5) {
                cout << "Invalid action. Please try again." << endl;
            }
        }

        switch (action) {
            case 0: // Help
                bookMove(yourHand, dealerHand);
                break;
            case 1: // Hit
                boot.transferCard(yourHand);
                bust = total(yourHand);
                break;
            case 2: // Stand
                total(yourHand);
                endTurn = true;
                break;
            case 3: // Double
                boot.transferCard(yourHand);
                bust = total(yourHand);
                endTurn = true;
                // End turn
                break;
            case 4: // Split
                if (yourHand[0].value == yourHand[1].value) {
                    cout << "Splitting..." << endl;
                    vector<Card> temp;

                    sendCard(yourHand, temp);

                } // Check if the cards match
                else {
                    cout << "You may only split when you have a pair." << endl;
                }
                break;
            case 5:
                cout << endl << "Counting..." << endl;
                this_thread::sleep_for(chrono::seconds(1));
                cout << "Chips remaining: " << chips << endl;
                break;
        }

        action = -1;
    }
    
    if (bust) {
        cout << "Busted :(" << endl;
        bet = 0;
    }
    else {
        cout << "didn't bust :)" << endl;
        bet = 0;
    }

    return;
}
*/

void blackjack() {
    int chips = 20, bet = 0, insurance = -1, numCards = 0;
    bool playing = false, placeInsurance = false;
    vector<Card> yourHand, dealerHand, discardPile;

    const int NUM_SEATS = 7;
    vector<vector<Card>> seats(NUM_SEATS); // A vector of vectors to represent seats

    Deck boot = Deck();
    Deck deck2 = Deck();
    Deck deck3 = Deck();
    Deck deck4 = Deck();

    // The game starts by having the dealer combine four decks, then shuffle them all into a boot
    boot.combine(deck2);
    boot.combine(deck3);
    boot.combine(deck4);

    cout << endl << "Shuffling deck..." << endl;
    this_thread::sleep_for(chrono::seconds(2));
    boot.shuffle();

    /*
    // Prompt the player to choose a seat
    int chosenSeat;
    cout << "Please choose a seat (1-" << NUM_SEATS << "): ";
    cin >> chosenSeat;

    
    // Validate the chosen seat
    while (chosenSeat < 1 || chosenSeat > NUM_SEATS && !(cin >> chosenSeat)) {
        // Clears the input buffer to prevent infinite loops
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        cout << "Invalid seat. Please choose a seat between 1 and " << NUM_SEATS << ": ";
        cin >> chosenSeat;
    }
    */

    // ********************************************************************* //

    cout << endl << "Welcome to Blackjack! Bets start at 5 chips and go up to 300." << endl;

    // Bets are placed initially
    while ((bet < 5) || (bet > 300) && !(cin >> bet)) {
        cout << "You have " << chips << " chips." << " How much will you bet?" << endl;
        cin >> bet;

        // Clears the input buffer to prevent infinite loops
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        
        // Let the player know what they did wrong
        cout << endl;
        if (bet < 5) { cout << "You must bet at least 5 chips to play!" << endl; }
        if (bet > 300) { cout << "You cannot bet more than 300 chips at this table!" << endl; }
    }

    chips -= bet;
    //cout << "All bets have been placed!" << endl;

    // Create a function that goes to all seats with players in them and bets placed, gives them a card
    // The dealer gives a single card to the players from right to left before placing their own card face down.
    boot.transferCard(yourHand);
    boot.transferCard(dealerHand);

    cout << "Your first card is " << yourHand[0] << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << "The dealer receives a face down card" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << endl;

    // The dealer then gives out a second card to all players, and places their second card face up.
    boot.transferCard(yourHand);
    boot.transferCard(dealerHand);

    cout << "Your second card is " << yourHand[1] << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << "The dealer has " << dealerHand[1] << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << endl;

    // If the dealer has an Ace, insurance can be placed by the players
    if (dealerHand[1].value == 1) {
        placeInsurance = true;

        cout << "UH OH!! Insurance time" << endl;
        cout << "The dealer will now take insurance" << endl;
        cin >> insurance;

        while ((insurance > bet) || (insurance < 0) && !(cin >> insurance)) {
            // Clears the input buffer to prevent infinite loops
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Let the player know what they did wrong
            if (insurance > bet) { cout << "You can only place up to half of your original bet in insurance." << endl << endl; }
            if (insurance < 0) {cout << "That is not a valid bet. Please try again." << endl << endl; }

            cin >> insurance;
        }
    }

    // Once insurance has been placed, the dealer will check the card. If they have blackjack, then those who placed insurance will receive 2:1 of their insurance bet. Otherwise, the bet is collected by the dealer and the game continues.
    if (placeInsurance) {
        if (dealerHand[0].value >= 10) {
            cout << "The dealer has blackjack! Those who bet insurance will be paid." << endl;
            chips += 2*insurance;
            this_thread::sleep_for(chrono::seconds(1));
        }
        else {
            cout << "The dealer does not have blackjack. Insurance has been collected" << endl;
            chips -= insurance;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    total(yourHand);

    //yourTurn();

    int action = -1;
    bool endTurn = false, bust = false;
    
    while (!endTurn && !bust) {
        // Perform an action on your turn
        cout << "What will you do?" << endl;
        cout << "Help [0], Hit [1], Stand [2], Double Down [3], Split [4], Count Chips [5]" << endl;

        while ((action < 0) || (action > 5) && !(cin >> action)) {
            cin >> action;

            // Clears the input buffer to prevent infinite loops
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (action < 0 || action > 5) {
                cout << "Invalid action. Please try again." << endl;
            }
        }

        switch (action) {
            case 0: // Help
                bookMove(yourHand, dealerHand);
                break;
            case 1: // Hit
                boot.transferCard(yourHand);
                bust = total(yourHand);
                break;
            case 2: // Stand
                total(yourHand);
                endTurn = true;
                break;
            case 3: // Double
                boot.transferCard(yourHand);
                bust = total(yourHand);
                endTurn = true;
                // End turn
                break;
            case 4: // Split
                if (yourHand[0].value == yourHand[1].value) {
                    cout << "Splitting..." << endl;
                    vector<Card> temp;

                    sendCard(yourHand, temp);

                } // Check if the cards match
                else {
                    cout << "You may only split when you have a pair." << endl;
                }
                break;
            case 5:
                cout << endl << "Counting..." << endl;
                this_thread::sleep_for(chrono::seconds(1));
                cout << "Chips remaining: " << chips << endl;
                break;
        }

        action = -1;
    }
    
    if (bust) {
        cout << "Busted :(" << endl;
        bet = 0;
    }
    else {
        cout << "didn't bust :)" << endl;
        bet = 0;
    }
    
    // If the player and dealer both have blackjack, then it is a push (a tie), and the player neither wins nor loses money on the hand.
    // If the game has continued, any players that have blackjack will be paid out immediately.

    // Then from right to left, the players can decide what they will do. 



    // Collects all of the cards from the seats
    sendAllCards(seats, discardPile);
    

    // Time to shuffle the boot!
    if (discardPile.size() > 160) {
        // Collect all cards from the table and shuffle
        cout << "It is time for a new boot!" << endl;
        boot.addCards(discardPile);
        boot.shuffle();       
    }

    // Check if the player has enough chips to play
    if (chips < 5) {
        int response;
        cout << "You don't have enough chips to play! Game over." << endl << "Enter [1] to restart the game, or [2] to return to the menu." << endl;
        cin >> response;
        
        while (!(cin >> response)) {
            if (response == 1) {
                playing = true;
                cout << "Restarting the game." << endl;
            } // Start the game over
            else if (response == 2) {
                cout << "Closing the game." << endl;
                playing = false;
            } // Close the game
            
            // Clear input buffer to prevent infinite loop
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            cin >> response;
        }
    }

    return;
}

int main() {
    /*
    Card card = Card(1, 3); // Creates an Ace of Diamonds
    Card card2 = Card(6, 1); // Creates a 6 of Clubs
    Card card3 = Card(5, 2); // Creates a 5 of Spades

    card.showCard();
    card2.showCard();
    card3.showCard();

    Deck deck = Deck();
    Deck deck2 = Deck();

    deck.showCards();
    deck.shuffle();
    deck.showCards();

    deck.combine(deck2);
    deck.shuffle();
    deck.showCards();
    */

    blackjack();

    int response = 0;
    bool start = false;

    while (!start) {
        cout << "What game would you like to play? Enter the corresponding number to select your choice." << endl;
        cout << "Blackjack [1]" << endl;

        cin >> response;

        if (response == 1) { start = true; }
    }

    switch (response) {
        case 1:
            blackjack();
            break;
        default:
            cout << "Unknown game! Please try again" << endl;
            break;
    }

    return 0;
}