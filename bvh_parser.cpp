#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include "bvh_parser.h"

using namespace std;

void populate_motion(joint &root, string line)
{
    stringstream ss(line);

    string motion;
    stack<joint *> s;

    s.push(&root);

    while (!s.empty())
    {
        joint *node = s.top();
        s.pop();

        for (auto it = node->children.rbegin(); it != node->children.rend(); it++)
        {
            s.push(*it);
        }

        vector<double> motions;

        for (auto channel : node->channels)
        {
            ss >> motion;
            motions.push_back(stod(motion));
        }

        node->motion.push_back(motions);
    }
}

int main(int argc, char **argv)
{
    joint root;
    META meta_data;
    ifstream file(argv[1]);

    if (!file.is_open())
    {
        cerr << "Error: Failed to open file." << endl;
        return 1;
    }

    string line;
    vector<joint *> nodeStack;

    nodeStack.push_back(&root);

    while (getline(file, line))
    {
        stringstream ss(line);

        string token;

        ss >> token;

        if (nodeStack.empty())
            break;

        if (token == "ROOT")
        {
            nodeStack.back()->joint_type = token;
            ss >> nodeStack.back()->name;
        }

        if (token == "JOINT")
        {
            joint *node = new joint;
            ss >> node->name;
            node->joint_type = token;
            nodeStack.back()->children.push_back(node);
            nodeStack.push_back(node);
        }

        else if (token == "End")
        {
            string buffer;
            ss >> buffer;
            joint *node = new joint;
            node->joint_type = token;
            node->name = nodeStack.back()->name + "_End";
            nodeStack.back()->children.push_back(node);
            nodeStack.push_back(node);
        }

        else if (token == "OFFSET")
        {
            ss >> nodeStack.back()->offset_x >> nodeStack.back()->offset_y >> nodeStack.back()->offset_z; // 载入节点的偏移量
        }

        else if (token == "CHANNELS")
        {
            size_t channel_size;
            ss >> channel_size;

            for (auto i = 0; i < channel_size; i++)
            {
                string channel;

                ss >> channel;
                nodeStack.back()->channels.push_back(channel);
            }
        }
        else if (token == "}")
        {
            nodeStack.pop_back();
        }
    }

    while (getline(file, line))
    {
        stringstream ss(line);

        string token;

        ss >> token;

        if (token == "Frames:")
        {
            ss >> meta_data.frame;
        }
        else if (token == "Frame")
        {
            ss >> token;
            ss >> meta_data.frame_time;
            break;
        }
    }
    while (getline(file, line))
    {
        populate_motion(root, line);
    }

    jsonify(root, meta_data);
    file.close();
    return 0;
}