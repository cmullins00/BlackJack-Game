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
 *      1. The player shall have the ability to place a bet, hit, double down, split, or stand
 *      2. The player shall be able to ask the dealer for the book move at any point on their turn
 *      3. The dealer shall use 4 decks
 *      4. The dealer shall shuffle the cards when the boot is empty or near empty
 *      5. The table shall have a minimum and maximum bet size
 *      6. The game should have some way to track or acknowledge the player's wins/losses
 *  
 *  Requirements Checklist:
 *      1. In Progress
 *      2. In Progress
 *      3. Completed
 *      4. Completed
 *      5. Completed
 *      6. Not Yet Started
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
        friend ostream& operator<<(ostream& os, const Card& card) 
        {
            // Define arrays to store rank and suit strings
            const string ranks[] = { "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King" };
            const string suits[] = { "Clubs", "Spades", "Diamonds", "Hearts" };

            // Check if rank and suit values are within the valid ranges
            if (card.value < 1 || card.value > 13 || card.suit < 1 || card.suit > 4) 
            {
                os << "Invalid card! Something went wrong.\n";
            }
            else 
            {
                // Output the formatted card information
                //os << ( card.value == 1 ? "an " : "a ") << ranks[card.value - 1] << " of " << suits[card.suit - 1];
                os << ranks[card.value - 1] << " of " << suits[card.suit - 1];
            }

            return os;
        }
};

// Class used for generating and managing a deck of cards
class Deck {
    public:
        // Generates one of each card and stores the result in a vector
        Deck() {}

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

        // Returns the number of cards in a deck
        int countCards() {
            return numCards;
        }

        // Transfers the top card from one deck to another
        void sendCard(Deck& destination) {
            if (cards.empty()) { // Check that our deck is not empty
                cout << "Deck is empty! Cannot transfer a card." << endl;
                return;
            } 
  
            destination.cards.push_back(cards[0]);  // Transfer the first card to the destination
            cards.erase(cards.begin());             // Remove the first card from the deck

            destination.numCards++;

            return;
        }

        void sendAllCards(Deck& destination) {
            while(numCards > 0) {
                sendCard(destination);
            }

            return;
        }

       // Displays all cards within a deck
        void getCards() const {
            for (const auto& card : cards) {
                cout << card << endl;
            }

            cout << "This deck has " << numCards << (numCards == 1 ? " card." : " cards." ) << endl;
            return;
        }

        // Displays a single cards from within a deck at the given index
        const Card& getCard(int index) const {
            if (index < 0 || index > numCards-1) {
                // Throws an exception
                throw out_of_range("Index is not within the valid range.");
            }

            return cards[index];
        }

        // Combines two decks of cards
        void combine(const Deck& deck2) {
            numCards += deck2.numCards;
            cards.insert(cards.end(), deck2.cards.begin(), deck2.cards.end());

            return;
        }

        // Shuffles a deck of cards
        void shuffle() {
            // Generate a new seed based on the current time
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();

            // Use the new seed to shuffle the cards
            std::shuffle(std::begin(cards), std::end(cards), default_random_engine(seed));

            cout << "The deck has been shuffled." << endl;
            return;
        }

        // Calculates and returns the total value in a player's hand
        int calculateTotal() const {
            int total1 = 0, total2 = 0;
            bool ace = false;

            for (const auto &card : cards) {
                if (card.value == 1) { // If an Ace is found
                    total1 += 11;
                    total2 += 1;
                    ace = true;
                } 
                else if (card.value > 10) { // If a face card is found
                    total1 += 10;
                    total2 += 10;
                } 
                else { // Default case
                    total1 += card.value;
                    total2 += card.value;
                }
            }

            // Returns total1 if it is <= 21, otherwise return total2
            return (total1 <= 21) ? total1 : total2;
        }

        bool isBusted() const {
            if (calculateTotal() > 21) { // Check if player has busted
                return true;
            }

            // Did not bust!
            return false;
        }

        // Used to calculate the winnings for a player
        int payout(int bet) {
            // If the player has blackjack
            if ((calculateTotal() == 21) && (numCards == 2)) {
                return bet + (bet * 1.5);
            }

            // If the player wins by regular means
            return (bet * 2);
        }

        void bookMove(Deck& dealerHand) {

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

    private:
        vector<Card> cards; // Stores all of the cards in the deck
        int numCards = 0;   // How many cards are within the deck
};

void blackjack() {
    int chips = 20, bet = 0, insurance = -1;
    bool playing = false, aceUpcard = false;
    Deck yourHand, dealerHand, discardPile;

    /*********************
     * Game setup
     *********************/

    // The boot shall contain 4 decks of cards, as stated in the requirements
    Deck boot = Deck();
    boot.fillDeck();
    boot.fillDeck();
    boot.fillDeck();
    boot.fillDeck();
    boot.shuffle();

    /*********************
     * Game start
     *********************/

    cout << endl << "Welcome to Blackjack! Bets start at 5 chips and go up to 300." << endl;

    // Bets are placed initially
    while ((bet < 5) || (bet > 300) && !(cin >> bet)) {
        cout << "You have " << chips << " chips." << " How much will you bet?" << endl;
        cin >> bet;

        // Clears the input buffer to prevent infinite loops
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Let the player know what they did wrong
        if (bet < 5) { cout << "You must bet at least 5 chips to play!" << endl; }
        if (bet > 300) { cout << "You cannot bet more than 300 chips at this table!" << endl; }
    }

    cout << endl;

    // Remove bet from player's total chips
    chips -= bet;

    // The dealer gives a single card to the player before placing their own card face down.
    boot.sendCard(yourHand);
    boot.sendCard(dealerHand);

    cout << "Player's hand: " << yourHand.getCard(0) << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << "Dealer's hand: [Face Down Card]" << endl;
    this_thread::sleep_for(chrono::seconds(1));
    cout << endl;

    // The dealer then gives out a second card, and places their second card face up.
    boot.sendCard(yourHand);
    boot.sendCard(dealerHand);

    cout << "Player's hand: " << yourHand.getCard(0) << ", " << yourHand.getCard(1) << endl; 
    this_thread::sleep_for(chrono::seconds(1));

    cout << "Dealer's hand: [Face Down Card], " << dealerHand.getCard(1) << endl;
    this_thread::sleep_for(chrono::seconds(1));
    cout << endl;

    // If the dealer has an Ace, insurance can be placed by the players
    if (dealerHand.getCard(1).value == 1) {
        aceUpcard = true;

        cout << "Dealer has an Ace. Insurance time!" << endl;
        cout << "Players can now place insurance if they would like to." << endl;

        while ((insurance > bet / 2) || (insurance < 0) && !(cin >> insurance)) {
            cin >> insurance;

            // Let the player know what they did wrong
            if (insurance > bet / 2) { cout << "You can only place up to half of your original bet in insurance." << endl << endl; }
            if (insurance < 0) {cout << "That is not a valid bet. Please try again." << endl << endl; }

            // Clears the input buffer to prevent infinite loops
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        chips -= insurance;
    }

    // Once insurance has been placed, the dealer will check the card. If they have blackjack, then those who placed insurance will receive 2:1 of their insurance bet. Otherwise, the bet is collected by the dealer and the game continues.
    if (aceUpcard) {
        if (dealerHand.getCard(1).value >= 10) {
            cout << "Dealer has Blackjack! Insurance paid." << endl;
            chips += 2*insurance;
            this_thread::sleep_for(chrono::seconds(1));
        }
        else {
            cout << "Dealer does not have Blackjack. Insurance lost." << endl;
            chips -= insurance;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    cout << "Your total: " << yourHand.calculateTotal() << endl;

    // This section should be turned into a function, maybe named playerTurn();
    int action = -1;
    bool endTurn = false, bust = false;
    
    while (!endTurn && !bust) {
        // Perform an action on your turn
        cout << "Choose an action:" << endl;
        cout << "[0] Book Move" << endl;
        cout << "[1] Hit" << endl;
        cout << "[2] Double Down" << endl;
        cout << "[3] Stand" << endl;

        // If the player has two cards with the same value, they can split
        if (yourHand.getCard(0).value == yourHand.getCard(1).value) {
            cout << "[4] Split" << endl;
        }

        cout << "[5] Count Chips" << endl;

        //cout << "Help [0], Hit [1], Stand [2], Double Down [3], Split [4], Count Chips [5]" << endl;
        cin >> action;

        while ((action < 0) || (action > 5) && !(cin >> action)) {
            cout << endl << "Invalid action. Please choose again." << endl;

            // Clears the input buffer to prevent infinite loops
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        cout << endl;

        switch (action) {
            case 0: // Check the book move
                yourHand.bookMove(dealerHand);
                break;
            case 1: // Hit
                boot.sendCard(yourHand);
                cout << "You have " << yourHand.calculateTotal() << "." << endl;
                bust = yourHand.isBusted();
                break;
            case 2: // Double down
                boot.sendCard(yourHand);
                cout << "You have " << yourHand.calculateTotal() << "." << endl << endl;
                bust = yourHand.isBusted();
                endTurn = true;
                // End turn
                break;
            case 3: // Stand
                cout << "You have " << yourHand.calculateTotal() << "." << endl << endl;
                endTurn = true;
                break;
            case 4: // Split
                if (yourHand.getCard(0).value == yourHand.getCard(1).value) {
                    cout << "Splitting..." << endl;
                    Deck secondHand;
                    yourHand.sendCard(secondHand);
                    this_thread::sleep_for(chrono::seconds(1));

                } // Check if the cards match
                else {
                    cout << "You may only split pairs." << endl;
                }
                break;
            case 5: // Count chips
                cout << "Chips: " << chips << endl;
                this_thread::sleep_for(chrono::seconds(1));
                break;
        }

        action = -1;
    }

    cout << "Dealer's hand: " << dealerHand.getCard(0) << ", " << dealerHand.getCard(1) << endl;
    cout << "Dealer's total: " << dealerHand.calculateTotal() << endl << endl;

    // Now the dealer must draw cards until they bust, reach 17-21, or have a total of 5 cards
    while ((!bust) && (dealerHand.calculateTotal() < 17) && (!dealerHand.isBusted()) && (dealerHand.countCards() < 5)) {
        boot.sendCard(dealerHand);

        cout << "Dealer draws: " << dealerHand.getCard(dealerHand.countCards() - 1) << endl;
        cout << "Dealer total: " << dealerHand.calculateTotal() << endl << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Check to see if the dealer busted
    if (dealerHand.isBusted()) {
        cout << "Dealer busted! You win!" << endl;
        chips += yourHand.payout(bet);
    }  else if (bust || yourHand.calculateTotal() < dealerHand.calculateTotal()) {
        cout << "You lose!" << endl;
    } else if (yourHand.calculateTotal() > dealerHand.calculateTotal()) {
        cout << "You win!" << endl;
        chips += yourHand.payout(bet);
    } else {
        cout << "Push! It's a tie." << endl;
        chips += bet;
    }
    
    bet = 0;
    // If the player and dealer both have blackjack, then it is a push (a tie), and the player neither wins nor loses money on the hand.
    // If the game has continued, any players that have blackjack will be paid out immediately.

    // Then from right to left, the players can decide what they will do. 



    // Collects all of the cards from the seats
    //sendAllCards(seats, discardPile);

    // Time to shuffle the boot!
    if (boot.countCards() < 50) {
        // Collect all cards from the table and shuffle
        cout << "It is time for a new boot!" << endl;
        discardPile.sendAllCards(boot);
        boot.shuffle();
    }

    // Check if the player has enough chips to play
    if (chips < 5) {
        cout << "Not enough chips to continue. Game over." << endl;
    }
    else {
        cout << "Continue playing? (Y/N)" << endl;

        char response;
        cin >> response;
        
        if (response == 'Y' || response == 'y') {
            blackjack();
        }
        else {
            cout << "Exiting the game." << endl;
        }
    }

    return;
}

int main() {
    blackjack();

    return 0;
}