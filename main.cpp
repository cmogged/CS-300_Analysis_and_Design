/*
 * Advising Assistance software
 *
 *  Date: 02-23-2025
 *  Author: Crailin Mogged
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct ABCU_Course {
   string courseNum;
   string courseName;
   vector<string> coursePrereq; //course prerequisite
};

// internal structure for tree node
struct Node {
    ABCU_Course ac;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }
};

struct BinaryTree {
    Node* root;
    
    // default constructor
    BinaryTree() {
        root = nullptr;
    }
};

void deleteNodes(Node* node)
{
    if (node != nullptr) // if node is not equal to null
    {
        deleteNodes(node->left);
        delete(node);
        deleteNodes(node->right);
    }
}

// prints course data in the order controlled by the addNode() function
void printCourseData(Node* node)
{
    if (node != nullptr) // if node is not equal to null
    {
        printCourseData(node->left);
        
        // print course data
        cout << node->ac.courseNum << ", " << node->ac.courseName << endl;
        
        for (unsigned int i=0; i < node->ac.coursePrereq.size(); i++)
        cout << " * " << node->ac.coursePrereq[i] << endl;
        
        cout << endl;
        
        printCourseData(node->right);
    }
}

void addNode(Node* node, ABCU_Course ac)
{
    if (node->ac.courseNum > ac.courseNum) // if greater than the given string (creates an alphanumeric order given only uppercase)
    {
        if (node->left == nullptr) // add to left subtree
        {
            node->left = new Node();
            node->left->ac = ac;
        }
        else
        addNode(node->left, ac);
        
    }
    else
    {
        if (node->right == nullptr) // add to right subtree
        {
            node->right = new Node();
            node->right->ac = ac;
        }
        else
        addNode(node->right, ac);
    }
}

Node *findCourse(Node* node, string& courseNum)
{
    Node *courseNode = nullptr;
    
    // if string in node is larger than the given string
    if (node->ac.courseNum > courseNum) 
    {
        if (node->left != nullptr)
        {
            if (node->left->ac.courseNum == courseNum) // if course found
            courseNode = node->left; // assign courseNode for return
            else
            courseNode = findCourse(node->left, courseNum);
        }
    }
    else
    {
        if (node->right != nullptr)
        {
            if (node->right->ac.courseNum == courseNum) // if course found
            courseNode = node->right; // assign courseNode for return
            else
            courseNode = findCourse(node->right, courseNum);
        }
    }
    
    return courseNode;
}

////////////////////////////// LOAD DATA //////////////////////////////
bool loadSingleLine(string line, ABCU_Course *ac)
{
   stringstream ss;
   string prereq;
   
   ss << line; // put the string into a stream for processing
   
   // put the first string into courseNum 
   if (!getline(ss, ac->courseNum, ','))
   return true;
   
   // put the second string into courseName
   if (!getline(ss, ac->courseName, ','))
   return true;
   
   ac->coursePrereq.clear(); // erase old data before reuse
   
   // read line string by string using a comma as delimiter
   while (getline(ss, prereq, ','))
   ac->coursePrereq.push_back(prereq);
   
   return false;
}

bool checkPrerequisites(Node *root, Node *node)
{
    bool error = false;
    bool found;
    
    if (node != nullptr) // if node is not equal to null
    {
        if (error == false)
        error = checkPrerequisites(root, node->left);
        
        // check if course prerequisite exists
        for (unsigned int i=0; i < node->ac.coursePrereq.size(); i++)
        {
            found = false;
            
            // look for prerequisite
            if (node->ac.coursePrereq[i] == root->ac.courseNum) // check root
            found = true;
            else if (findCourse(root, node->ac.coursePrereq[i]))
            found = true;
            
            if (!found) // if prerequisite not found return error
            {
                cout << "A prerequisite was not found." << endl;
                error = true;
                break;
            }
        }
        
        if (error == false)
        error = checkPrerequisites(root, node->right);
    }
    
    return error;
}

bool loadCourseData(BinaryTree *bt, string filename)
{
   ABCU_Course ac;
   string line;
   
   ifstream inputFile(filename);
   
   if (!inputFile) //check for open file error
   return true; //return true if error
   
   if (getline(inputFile, line)) // read first line
   if (loadSingleLine(line, &ac))
   return true;
   
   bt->root = new Node(); // create the root node
   bt->root->ac = ac;

   // read file line by line
   while (getline(inputFile, line))
   {
      if (loadSingleLine(line, &ac))
      return true;
      
      addNode(bt->root, ac);
   }
   
   if (checkPrerequisites(bt->root, bt->root))
   return true;
   
   inputFile.close(); // close file
   
   return false;
}
////////////////////////////// END OF LOAD DATA //////////////////////////////


////////////////////////////// USER INTERFACE //////////////////////////////
int getMenuOption()
{
    int optionNum;
    
    while (true)
    {
        if (cin >> optionNum) // check if cin successfully read an integer
        {
            if (((optionNum >= 0) && (optionNum <= 3)) || (optionNum <= 9)) // is the integer valid
            {
                while (cin.peek() != '\n') // ignore any other input
                cin.ignore();
                
                cin.ignore(); // ignore final '\n' character
                break;
            }
        }
        
        cout << "Invalid input. Please try again." << endl;
        cin.clear();
        
        while (cin.peek() != '\n') // ignore the current input
        cin.ignore();
    }
    
    return optionNum;
}

void continueOnEnter()
{
    cout << "Press [Enter] to continue." << endl;

    while (cin.peek() != '\n') // ignore the current input
    cin.ignore();
}

int main()
{
    int exit = false;
    BinaryTree bt;
    string courseDataFile;
    string courseNum;
    Node *courseNode;
    Node *prereqNode;
    
    while (exit == false)
    {
        cout << "\nMain Menu" << endl;
        cout << "Enter '1' to load file data." << endl;
        cout << "Enter '2' to print all course data." << endl;
        cout << "Enter '3' to find a specific course." << endl;
        cout << "Enter '9' to exit program." << endl;
        cout << endl;

        switch (getMenuOption()) {
        case 1:
            cout << "Enter the course data filename: ";
            getline(cin, courseDataFile); // read string from input
            cout << "Please wait..." << endl;
            
            deleteNodes(bt.root);
            bt.root = nullptr;
            if (loadCourseData(&bt, courseDataFile)) // load data
            {
                cout << "There was an error loading course data." << endl;
                deleteNodes(bt.root);
                return 0;
            }
            
            cout << "\"" << courseDataFile << "\" is now loaded." << endl;
            continueOnEnter();
            break;
        case 2:
            printCourseData(bt.root); // print data in the order in which it was loaded (alphanumeric)
            continueOnEnter();
            break;
        case 3:
            if (!bt.root)
            {
                cout << "No data loaded." << endl;
                continueOnEnter();
                break;
            }
            
            cout << "Enter course number: ";
            getline(cin, courseNum); // read string from input
            
            if (bt.root->ac.courseNum == courseNum) // check root
            courseNode = bt.root;
            else
            courseNode = findCourse(bt.root, courseNum);
    
            if (courseNode)
            {
                // print course data
                cout << courseNode->ac.courseName << endl;
                
                for (unsigned int i=0; i < courseNode->ac.coursePrereq.size(); i++)
                {
                    prereqNode = findCourse(bt.root, courseNode->ac.coursePrereq[i]);
                    cout << " * " << prereqNode->ac.courseNum << ", " << prereqNode->ac.courseName << endl;
                }
                
                cout << endl;
            }
            else
            cout << "Course does not exist." << endl;
    
            continueOnEnter();
            break;
        case 9:
            exit = true;
            break;
        }
    }
    
    deleteNodes(bt.root);
    return 0;
}
////////////////////////////// END OF USER INTERFACE //////////////////////////////

