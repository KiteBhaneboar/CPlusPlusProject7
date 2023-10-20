#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <numeric>


using namespace std;

ifstream inFile;
string line;
char separator[] = " ";
char* lineToCharPtr;
char* tokenPtr;
char* tokenPosition;
int lengths[4] = { 0, 0, 0, 0 };
int userWordLength = 0;
string wordArrays[4][16];




// 0101 if variable != None | variale != ErrorType::Substitution
enum ErrorType
{
	None,
	Substitution = 1,
	Transposition = 2,
	Deletion = 4,
	Insertion = 8,
	TooOff = 16
};

string fileArrays() {

	inFile.open("mp7spelling.txt");
	if (inFile.fail()) {
		return "No File Found";
	}
	int lineNum = 0;
	while (!inFile.eof()) {
		getline(inFile, line);
		lineToCharPtr = (char*)line.c_str();
		tokenPtr = strtok_s(lineToCharPtr, " ", &tokenPosition);

		while (tokenPtr != NULL) {
			wordArrays[lineNum][lengths[lineNum]] = tokenPtr;
			tokenPtr = strtok_s(NULL, separator, &tokenPosition);
			lengths[lineNum]++;
		}
		//for (int i = 0; i < lengths[lineNum]; i++)     //debugging
		//	cout << wordArrays[lineNum][i] << setw(3) << setfill(' ') << endl;
		lineNum++;
	}
	return "";
};


void checkTransposition(std::string& candidateWord, std::string& actualWord, ErrorType& errorType, int& transposition)  //transposition
{
	for (int letter = 1; letter < candidateWord.length(); letter++)
	{
		if (candidateWord[letter] == actualWord[letter - 1] && candidateWord[letter - 1] == actualWord[letter])
		{
			if ((ErrorType)(errorType & ErrorType::Transposition) == ErrorType::Transposition)
				errorType = (ErrorType)(errorType | ErrorType::TooOff);
			else
			{
				errorType = ErrorType::Transposition;
				transposition++;
			}
		}
	}
}


void checkDeletion(std::string& candidateWord, int& offset, std::string& actualWord, bool& offByOne, ErrorType& errorType, int& deletion) //deletion
{
	if (candidateWord.length() < actualWord.length())
	{
		for (int letter = 0; letter < actualWord.length() && offset < candidateWord.length(); letter++)
		{
			if (candidateWord[offset] == actualWord[letter + 1])
			{
				if (offByOne)
				{
					errorType = (ErrorType)(errorType | ErrorType::TooOff);
				}
				offByOne = true;
				offset--;
			}
			else if (candidateWord[offset] != actualWord[letter])
				errorType = (ErrorType)(errorType | ErrorType::TooOff);
			offset++;
		}

		if (offByOne)
		{
			errorType = (ErrorType)(errorType | ErrorType::Deletion);
			deletion++;
		}
		else if (errorType != ErrorType::TooOff)
		{
			errorType = (ErrorType)(errorType | ErrorType::Deletion);
			deletion++;
		}

	}
}


void checkInsertion(int& offset, std::string& candidateWord, std::string& actualWord, bool& offByOne, ErrorType& errorType, int& insertion) //insertion
{
	offset = 0;
	if (candidateWord.length() > actualWord.length())
	{
		for (int letter = 0; letter < actualWord.length() && offset < candidateWord.length(); letter++)
		{
			if (candidateWord[offset + 1] == actualWord[letter])
			{
				if (offByOne)
				{
					errorType = (ErrorType)(errorType | ErrorType::TooOff);
				}
				offByOne = true;
				offset++;
			}
			else if (candidateWord[offset] != actualWord[letter])
				errorType = (ErrorType)(errorType | ErrorType::TooOff);
			offset++;
		}
		if (offByOne)
		{
			errorType = (ErrorType)(errorType | ErrorType::Insertion);
			insertion++;
		}
		else if (errorType != ErrorType::TooOff)
		{
			errorType = (ErrorType)(errorType | ErrorType::Insertion);
			insertion++;
		}
	}
}


void checkSubstitution(std::string& candidateWord, std::string& actualWord, ErrorType& errorType, int& substitution) //substitution
{
	int i = 0;
	for (int letter = 0; letter < candidateWord.length(); letter++)
	{
		if (candidateWord[letter] != actualWord[letter]) {
			i++;
		}
	}
	if (i == 1)
	{
		errorType = (ErrorType)(errorType | ErrorType::Substitution);
		substitution++;
	}
	else if (i > 1)
		errorType = (ErrorType)(errorType | ErrorType::TooOff);

}


void checkMistakes() {
	int correct = 0;
	int transposition = 0;
	int substitution = 0;
	int insertion = 0;
	int deletion = 0;
	int notMispelling = 0;

	for (int lineIndex = 0; lineIndex < 4; lineIndex++)
	{
		string actualWord = wordArrays[lineIndex][0];
		cout << endl << endl;
		cout << "The original word is " << actualWord << endl << endl;
		for (int wordIndex = 1; wordIndex < lengths[lineIndex]; wordIndex++)
		{
			ErrorType errorType = ErrorType::None;
			string candidateWord = wordArrays[lineIndex][wordIndex];
			int offset = 0;
			bool offByOne = false;
			if (actualWord == candidateWord) //correct
			{
				correct++;
			}
			else
			{
				checkTransposition(candidateWord, actualWord, errorType, transposition);
				if (errorType == ErrorType::None)
					checkDeletion(candidateWord, offset, actualWord, offByOne, errorType, deletion);
				if (errorType == ErrorType::None)
					checkInsertion(offset, candidateWord, actualWord, offByOne, errorType, insertion);
				if (errorType == ErrorType::None)
					checkSubstitution(candidateWord, actualWord, errorType, substitution);
			}
			if (errorType != ErrorType::Transposition &&
				errorType != ErrorType::Substitution &&
				errorType != ErrorType::Insertion &&
				errorType != ErrorType::Deletion &&
				errorType != ErrorType::None)
			{
				errorType = (ErrorType)(errorType | ErrorType::TooOff);
				notMispelling++;
			}

			if ((ErrorType)(errorType & ErrorType::TooOff) == ErrorType::TooOff)
				cout << candidateWord << " is too far off to be a misspelling!" << endl;
			else if ((ErrorType)(errorType & ErrorType::Transposition) == ErrorType::Transposition)
				cout << candidateWord << " is an error type of transposition." << endl;
			else if ((ErrorType)(errorType & ErrorType::Deletion) == ErrorType::Deletion)
				cout << candidateWord << " is an error type of deletion." << endl;
			else if ((ErrorType)(errorType & ErrorType::Insertion) == ErrorType::Insertion)
				cout << candidateWord << " is an error type of insertion." << endl;
			else if ((ErrorType)(errorType & ErrorType::Substitution) == ErrorType::Substitution)
				cout << candidateWord << " is an error type of substitution." << endl;
			else
				cout << candidateWord << " is correct!" << endl;


		}
	}
	cout << endl << endl;
	cout << "Correct Spellings: " << setw(25) << setfill('-') << correct << endl;
	cout << "Substitutions: " << setw(29) << setfill('-') << substitution << endl;
	cout << "Transpositions: " << setw(28) << setfill('-') << transposition << endl;
	cout << "Insertions: " << setw(32) << setfill('-') << insertion << endl;
	cout << "Deletions: " << setw(33) << setfill('-') << deletion << endl;
	cout << "Too Far Off: " << setw(31) << setfill('-') << notMispelling << endl;

};

int main() {

	fileArrays();
	checkMistakes();



	return 0;
}