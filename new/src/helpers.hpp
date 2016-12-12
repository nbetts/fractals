/**
 * [program description]
 */

#ifndef HELPER_HEADER
#define HELPER_HEADER

#include <map>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

/**
 * Return a random real number in the given range.
 */
GLfloat randomNumber(GLfloat min, GLfloat max)
{
  return min + rand() / (RAND_MAX / (max - min) + 1.0f);
}

/**
 * Calculate the average value of a list of values.
 */
GLfloat average(std::initializer_list<GLfloat> values)
{
  GLfloat sum = 0.0f;
  GLuint count = 0;

  for(auto value : values) {
    sum += value;
    count++;
  }

  return sum / (GLfloat)count;
}

/**
 * Read the whole content of a given file into a char array.
 */
const GLchar* readFile(const GLchar* filename)
{
  using namespace std;

  ifstream file(filename);
  stringstream buffer;
  string* contents;

  buffer << file.rdbuf();
  file.close();
  contents = new string(buffer.str());

  return contents->c_str();
}

/**
 * Parse a line from the given profile to retrieve variable names and values.
 *
 * This solution to split a string is my adaption of a generic solution that
 * originates from Stack Overflow, specifically here:
 * http://stackoverflow.com/a/236803
 *
 * The original answer to the question was provided by the following author:
 * http://stackoverflow.com/users/13430/evan-teran
 *
 * The code is licensed under CC-BY-SA:
 * https://creativecommons.org/licenses/by-sa/3.0/
 */
std::vector<std::string> parseLine(const std::string &line)
{
  using namespace std;

  vector<string> variables;
  stringstream stream;
  stream.str(line);
  string lineItem;

  while (getline(stream, lineItem, '\n')) {
    if (lineItem[0] != ' ' && lineItem[0] != '#') {
      stringstream ssItem;
      ssItem.str(lineItem);
      string item;

      while (getline(ssItem, item, ' ')) {
        if (item.size() > 0 && item[0] == '#') {
          break;
        } else if (item.size() > 0 && item[0] != ' ') {
          variables.push_back(item);
        }
      }
    }
  }

  return variables;
}

/**
 * Read in environment configuration data from a given profile.
 */
std::map<std::string, GLfloat> readProfile(const GLchar* filename)
{
  using namespace std;

  ifstream profile(filename);
  string line;
  map<string, GLfloat> environmentVariables;

  if (!profile.is_open()) {
    printf("failed to open file: %s\n", filename);

    exit(EXIT_FAILURE);
  }

  while (getline(profile, line)) {
    vector<string> variables = parseLine(line);

    for (GLuint i = 0; i < variables.size(); i += 2) {
      environmentVariables[variables[i]] = stof(variables.at(i+1));
    }
  }

  profile.close();

  return environmentVariables;
}

#endif
