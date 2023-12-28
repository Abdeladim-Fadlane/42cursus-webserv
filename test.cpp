#include <iostream>
#include <sstream>
#include <string>
using std::istringstream;
using std::string;
using std::cout;

// Driver Code
int main()
{
	// Input string
	string str("1, 2, 3");

	// Object class of istringstream
	istringstream my_stream(str);

	// Variable to store the number n
	// and character ch
	char c;
	int n;

	// Traverse till input stream is valid
	while (my_stream >> n >> c) {

		cout << "That stream was successful: "
			<< n << " " << c << "\n";
	}
	cout << "The stream has failed."
		<< "\n";

	return 0;
}