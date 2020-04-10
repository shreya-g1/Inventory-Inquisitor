//InvInq.cpp
//Shreya Gowda, CISP 400
//10/13/19

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <fstream>
using namespace std;

//options for menu
typedef enum {START, ADD, DELETE, EDIT, DISPLAY, QUIT} option_type;

//class to check for invalid input 
//uses throw-try-catch method
class InvalidInput{
  private:
  int data;
  int max_val;
  int min_val;

  public:
  //constructor
  InvalidInput(){
    min_val=0;
    max_val=0;
    data=0;
  }

  //exceptions
  class OutOfRange{};
  class NotCin{};

  void checkCin(){
    /*checks if input is compatible with type. clears input and throws exception if not.*/

    string dump="";  //used with getline to clear input
    if(!cin){
      cin.clear();
      getline(cin, dump);  
      throw NotCin();
    }
  }

  void checkRange(int min, int max, int data){
    /*checks if integer input is within valid range of number. throws exception if not*/

    min_val=min;
    max_val=max;
    if(data<min||data>max){
      throw OutOfRange();
    }
  }

  //overloaded function. see above function
  void checkRange(int min, int data){
    min_val=min;
    if(data<min){
      throw OutOfRange();
    }
  }
};

// Specification B1 - OOP
//holds information about one item in an inventory (description, quantity, cost, date)
class Item {
  private:
  string description;
  int quantity;
  double cost;
  time_t date;

  public:
  Item(){
    description="";
    quantity=0;
    cost=0.0;
    date= time(NULL);
  }

  void set_description(string s){
    description=s;
  }

  void set_quantity(int q){
    quantity=q;
  }

  void set_cost(double c){
    const double markup=1.00;  //markup for calculating retail cost set to 100%
    //calculating retail cost after markup.
    cost=c+c*markup;
  }

  string get_description(){
    return description;
  }

  int get_quantity(){
    return quantity;
  }

  double get_cost(){
    return cost;
  }

  string get_date(){
    //formats and returns date

    tm* date_info = localtime(&date); 

    //individual strings w/ info on date
    string month=to_string((date_info->tm_mon)+1);
    string day=to_string(date_info->tm_mday);
    string year=to_string((date_info->tm_year)+1900);

    //concatenates individual strings to form whole date
    string formatted_date=month+"/"+day+"/"+year;

    return formatted_date;
  }
};

//creates, manipulates, and displays an inventory
class Inventory{
  private:
  const int min_quantity=0;
  const int min_char=0;   //min char length for description of an item
  const int max_char=22;  //max char length for description of an item
  const int min_inv_length=0;  //min # of items in inventory
  int num_items;     //# of items in inventory
  Item* inventory;
  ofstream logfile;  //file to log inventory

  public:
  Inventory():logfile("InventoryData.txt"){
    num_items=0;
    inventory=new Item[num_items];
    logfile<<"Inventory Data Tracked Changes"<<endl<<endl;
  }

  // Specification C2 - Add Option
  void add(){
    //adds an item to inventory

    Item* temp_inv;
    
    // Specification C1 - Dynamic Array
    //copies previous inventory objects into temporary array
    temp_inv=new Item[num_items];
    for(int i=0; i<num_items;i++){
      temp_inv[i]=inventory[i];
    }

    //deletes old array and makes new one which is bigger
    delete [] inventory;
    num_items++;
    inventory=new Item[num_items];
      
    //copies previous inventory items in temp array to the actual array.
    for(int i=0; i<num_items-1; i++){
      inventory[i]=temp_inv[i];
    }

    delete [] temp_inv;

    set_item_vals(num_items);

    logfile<<"Added An Item"<<endl;
    log_data();
  }

  //Specification B2 - Delete Inventory Item
  void remove(){
    //removes a specified item in inventory

    Item* temp_inv;
    int del_item=0;  //item # to delete
  
    cout<<endl<<"Item # to delete?"<<endl;
    del_item=num_validation();

    // Specification B3 - Shrink Dynamic Array
    //copies previous inventory objects into temporary array
    temp_inv=new Item[num_items];
    for(int i=0; i<num_items;i++){
      temp_inv[i]=inventory[i];
    }

    //deletes old array and makes new one which is smaller
    delete [] inventory;
    num_items--;
    inventory=new Item[num_items];

    /*copies prev inv items (from either side of del_item, 
    but not including) from temp array to actual array*/
    for(int i=0; i<del_item-1; i++){
      inventory[i]=temp_inv[i];
    }
    for(int i=del_item; i<=num_items; i++){
      inventory[i-1]=temp_inv[i];
    }

    delete [] temp_inv;

    logfile<<"Deleted Item # "<<del_item<<endl;
    log_data();
  }

  // Specification A1 - Edit Inventory
  void edit(){
    //edits a specified item in inventory 

    int edit_item=0;  //item # to edit

    cout<<endl<<"Item # to edit?"<<endl;
    edit_item=num_validation();

    set_item_vals(edit_item);

    logfile<<"Edited Item # "<<edit_item<<endl;
    log_data();
  }

  void set_item_vals(int set_item){
    //sets description, quantity, and cost for specified item
    //used for add and edit member functions

    string input=""; //user's input to questions
    string dump="";  //used with getline to clear input
    InvalidInput checkItem;  //uses invalidinput class to check inputs
    bool bad_input=false;    //turns true when invalid input (for do-while looping)

    // Specification A2 - Validate Inventory
    //input and validate description
    do{
      cout<<endl<<"Item Description? Input word of at most 22 characters."<<endl;
      try{
        //tries to input input and check description length (0-22)
        bad_input=false;
        cin>>input;
        checkItem.checkRange(min_char, max_char, input.length());
        inventory[set_item-1].set_description(input);
      }
      catch(InvalidInput::OutOfRange){
        //if exception thrown (invalid length), outputs message and loops again
        bad_input=true;
        cout<<"Invalid input, out of range (0-22 chars). Please Retype."<<endl;
      }
    }while(bad_input);

    //input and validate quantity
    do{
      cout<<endl<<"Quantity of Item? Input integer greater than or equal to 0."<<endl;
      try{
        //tries to input input and check integer >= 0
        bad_input=false;
        cin>>input;
        checkItem.checkRange(min_quantity, stoi(input));
        inventory[set_item-1].set_quantity(stoi(input));
      }
      catch(std::invalid_argument){
        /*if stoi() throws exception (because input was not integer): 
        clears input, outputs message, and loops again*/
        bad_input=true;
          cin.clear();
          getline(cin, dump); 
        cout<<"Invalid input, non-integer. Please Retype."<<endl;
      }
      catch(InvalidInput::OutOfRange){
        //if exception thrown (integer < 0) outputs message and loops again
        bad_input=true;
        cout<<"Invalid input, negative number. Please Retype."<<endl;
      }
    }while(bad_input); 

    //input and validate cost
    do{
      cout<<endl<<"Cost of Item? Input integer/decimal number."<<endl;
      try{
        //tries to input input
        bad_input=false;
        cin>>input;
        inventory[set_item-1].set_cost(stod(input));
      }
      catch(std::invalid_argument){
        /*if stoi() throws exception (because input was not 
        integer/decimal):
        clears input, outputs message, and loops again*/
        bad_input=true;
          cin.clear();
          getline(cin, dump); 
        cout<<"Invalid input, non-integer. Please Retype."<<endl;
      }
    }while(bad_input); 
  }

  // Bulletproof when inputting item #
  int num_validation(){
    //checks input and returns it. used for the remove and edit member functions

    InvalidInput checkItem;  //uses invalidinput class to check inputs
    bool bad_input=false;    ////turns true when invalid input (for do-while looping)
    int set_item=0;          //item # in array

    do{
      cout<<"Item #: ";
      try{
        /*tries to input input and check if input is integer 
        and within range (0-num_items)*/
        bad_input=false;
        cin>>set_item;
        checkItem.checkCin();
        checkItem.checkRange(min_inv_length+1, num_items, set_item);
      }
      catch(InvalidInput::NotCin){
        //exception if input is non-integer. outputs message and loops again
        bad_input=true;
        cout<<"Invalid input, non-integer. Please Retype."<<endl;
      }
      catch(InvalidInput::OutOfRange){
        //exception if integer is not between 0-num_items. outputs message and loops again
        bad_input=true;
        cout<<"Invalid input, out of range ("<<min_inv_length+1<<"-"<<num_items<<"). Please Retype."<<endl;
      }
    }while(bad_input);

    return set_item;
  }

  // Specification C3 - Display Option
  void display(){
    //displays contents of inventory in formatted manner on console

    //outputs table header
    cout<<endl;
    cout<<left<<setw(5)<<"#"<<setw(12)<<"Date"<<setw(24)<<"Description"<<setw(10)<<"Quantity"<<setw(10)<<"Cost"<<endl;

    //outputs all item contents
    if(num_items==min_inv_length){
      cout<<left<<setw(5)<<"N/A"<<setw(12)<<"N/A"<<setw(24)<<"N/A"<<setw(10)<<"N/A"<<setw(10)<<"N/A"<<endl;
    }
    for(int i=0; i<num_items; i++){
      cout<<left<<setw(5)<<i+1;
      cout<<left<<setw(12)<<inventory[i].get_date();
      cout<<left<<setw(24)<<inventory[i].get_description();
      cout<<left<<setw(10)<<inventory[i].get_quantity();
      cout<<left<<"$"<<setw(10)<<inventory[i].get_cost();
      cout<<endl;
    }
  }

  // Specification A3 - Logfile
  void log_data(){
    /*logs contents of inventory after every change. 
    used in add, remove, and edit member functions*/

    //logs table header
    logfile<<endl;
    logfile<<left<<setw(5)<<"#"<<setw(12)<<"Date"<<setw(24)<<"Description"<<setw(10)<<"Quantity"<<setw(10)<<"Cost"<<endl;

    //logs all item contents
    if(num_items==min_inv_length){
      logfile<<left<<setw(5)<<"N/A"<<setw(12)<<"N/A"<<setw(24)<<"N/A"<<setw(10)<<"N/A"<<setw(10)<<"N/A"<<endl;
    }
    for(int i=0; i<num_items; i++){
      logfile<<left<<setw(5)<<i+1;
      logfile<<left<<setw(12)<<inventory[i].get_date();
      logfile<<left<<setw(24)<<inventory[i].get_description();
      logfile<<left<<setw(10)<<inventory[i].get_quantity();
      logfile<<left<<"$"<<setw(10)<<inventory[i].get_cost();
      logfile<<endl;
    }
    logfile<<endl;
  }

  //destructor
  ~Inventory(){
    delete [] inventory;
    logfile.close();
  }
};

// Function Prototype
int menu_display();

int main() {
  Inventory inv;
  string user_input="";
  int menu_opt=0;  //stores user input for menu options

  // Program Greeting
  cout<<"Inventory Inquisitor"<<endl<<endl;
  cout<<"Welcome to Inventory Inquisitor, your go to program for all your inventory management needs.\n\nPress enter to begin";
  cin.ignore();

  while(option_type(menu_opt)!=QUIT){

    // Specification C5 - Validate Menu
    try{
      menu_opt=menu_display();
    }
    catch(InvalidInput::OutOfRange){
      cout<<"Invalid input, out of range (1-5). Please Retype.";
      menu_opt=0;
      cin.ignore();
    }
    catch(InvalidInput::NotCin){
      cout<<"Invalid input, non-integer. Please Retype.";
      menu_opt=0;
    }

    if(option_type(menu_opt)==ADD){
      inv.add();
      cin.ignore();
    }

    if(option_type(menu_opt)==DELETE){
      inv.remove();
      cin.ignore();
    }

    if(option_type(menu_opt)==EDIT){
      inv.edit();
      cin.ignore();
    }

    if(option_type(menu_opt)==DISPLAY){
      inv.display();
    }

    //for better pauses and prettier display output
    if(option_type(menu_opt)!=DISPLAY){
      cout<<endl<<"Press enter to continue";
      cin.ignore();
      system("clear");
    }
  }

  cout<<"Thanks for using Inventory Inquisitor!"<<endl;
}

// Specification C4 - Main Menu
int menu_display(){
  //displays menu of all options to manipulate inventory

  int option=0;          //user inputs menu option
  InvalidInput checker;  //uses invalidinput class to check inputs
  const int min=1;  //first available menu choice
  const int max=5;  //last available menu choice
  
  //menu display output
  cout<<endl<<"Enter the number corresponding with the menu option desired."<<endl<<endl;
  cout<<"Menu"<<endl;
  cout<<"(1) Add Inventory"<<endl;
  cout<<"(2) Delete Inventory"<<endl;
  cout<<"(3) Edit Inventory"<<endl;
  cout<<"(4) Display Inventory"<<endl;
  cout<<"(5) Quit"<<endl;
  cout<<"enter: ";

  cin>>option;

  //checks if input is integer and within range (1-5)
  checker.checkCin();
  checker.checkRange(min, max, option);

  return option;
}

