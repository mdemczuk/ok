#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

int V = 100;              // number of vertices
int x = 40;              // penalty value
float MAX = 170;
clock_t start, finish;

void instanceGenerator(int number);
void saveInstance(int number, vector<vector<int> > edges);
void loadInstance(int number, vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix);
void antsFirstIt(vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix, vector<vector<float> > &PheromoneMatrix);
void antsIterations(vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix, vector<vector<float> > &PheromoneMatrix, int iteration);
void pheromoneEvaporation(vector<vector<float> > &PheromoneMatrix);
void pheromoneSmoothing(vector<vector<float> > &PheromoneMatrix);

vector<int> buff;
vector<int> allValues;
vector<int> itValues;
int itValue;
int numberOfInstances;
int main() {

    srand(static_cast<unsigned>(time(0)));

    cout << "Wybierz modul, ktory chcesz uruchomic:\n1. Generator instancji\n2. Wczytanie instancji i ACO" << endl;
    int choice;
    cin >> choice;
    if(choice == 1) {
        cout << "Ile instancji chcesz wygenerowac?" << endl;
        cin >> numberOfInstances;
        int count = 1;
        while(count <= numberOfInstances) {
            instanceGenerator(count);
            count++;
        }
    }

    if(choice == 2){
        cout << "Ile instancji chcesz wczytac?" << endl;
        cin >> numberOfInstances;
        for(int j = 1; j <= numberOfInstances; j++){
            cout << "Instancja: " << j << endl;
            vector<vector<int> > Graph;
            vector<vector<int> > WeightMatrix;
            vector<vector<float> > PheromoneMatrix;
            loadInstance(j, Graph, WeightMatrix);

            int iteration = 1;
            start = clock();
            while(1){
                if(((clock() - start) / CLOCKS_PER_SEC) >= 20){ // if(... / >= 10 + i * 5
                    finish = clock();
                    break;
                }
                cout << "Iteracja " << iteration << endl;
                if(iteration == 1){
                    antsFirstIt(Graph, WeightMatrix, PheromoneMatrix);
                }
                if(iteration >= 2){
                    if(iteration >= 3){
                        pheromoneEvaporation(PheromoneMatrix);
                        pheromoneSmoothing(PheromoneMatrix);
                    }
                    antsIterations(Graph, WeightMatrix, PheromoneMatrix, iteration);
                }
                iteration++;                // na sam koniec petli
            }
            int bestValue = itValues[0];
            for(int k = 0; k < itValues.size(); k++){
                if(bestValue > itValues[k])
                    bestValue = itValues[k];
            }
            allValues.push_back(bestValue);
            cout << bestValue << endl;
            itValues.clear();
            double time = (double)(finish - start) / (double)(CLOCKS_PER_SEC);
            cout << "Czas: " << time << endl;
        }
        float average = 0;
        int val;
        for(int i = 0; i < allValues.size(); i++){
            val = allValues[i];
            average += (float)val;
        }
        average = average / numberOfInstances;
        cout << "Sredni wynik wynosi " << average << endl;
    }
    return 0;
}

void instanceGenerator(int number){
    vector<int> VerticesDegrees;

    int degree;
    for(int i = 1; i <= V; i++){
        degree = (rand() % (6 + 1 - 1)) + 1;
        VerticesDegrees.push_back(degree);
    }

    vector<int> deg1;
    vector<int> deg2;
    vector<int> degrees;
    int inx;
    for(int i = 0; i < V; i++){
        inx = VerticesDegrees[i];
        if(inx > 1) {
            deg2.push_back(i + 1);
            degrees.push_back(inx);
        }
        else {
            deg1.push_back(i + 1);
            degrees.push_back(0);
        }
    }

    random_shuffle(deg1.begin(), deg1.end());
    random_shuffle(deg2.begin(), deg2.end());

    vector<vector<int> > edg;
    buff.clear();
    int weight;
    for(int i = 0; i < deg2.size(); i++){               // tworzenie krawedzi cz. 1
        weight = (rand() % (100 + 1 - 1)) + 1;
        if(i != deg2.size()-1){
            buff.push_back(deg2[i]);
            buff.push_back(deg2[i+1]);
            buff.push_back(weight);
            degrees[deg2[i]-1]--;
            degrees[deg2[i+1]-1]--;
            edg.push_back(buff);
            buff.clear();
        }
        else{
            buff.push_back(deg2[i]);
            buff.push_back(deg2[0]);
            buff.push_back(weight);
            degrees[deg2[i]-1]--;
            degrees[deg2[0]-1]--;
            edg.push_back(buff);
            buff.clear();
        }
    }

    inx = 0;
    buff.clear();
    deg2.clear();
    for(int i = 0; i < degrees.size(); i++){                // tworzenie krawedzi cz.2: laczenie wierzcholkow o stopniu 1 z wczesniej powstalym prostym grafem
        if(degrees[i] > 0 && inx < deg1.size()){
            weight = (rand() % (100 + 1 - 1)) + 1;
            buff.push_back(i+1);
            buff.push_back(deg1[inx]);
            buff.push_back(weight);
            edg.push_back(buff);
            buff.clear();
            inx++;
            degrees[i]--;
        }
        if(degrees[i] > 0)
            deg2.push_back(i+1);
    }


    random_shuffle(deg2.begin(), deg2.end());
    buff.clear();

    for(int i = 0; i < deg2.size(); i++) {                              // tworzenie krawedzi cz.3
        if (deg2[i] > 0) {
           for(int j = i+1; j < deg2.size(); j++){
                int count = 0;
                if(deg2[j] != 0){
                    for(int k = 0; k < edg.size(); k++){
                        if(deg2[i] == edg[k][0]){
                            if(deg2[j] == edg[k][1])
                                count++;
                        }
                        else if(deg2[i] == edg[k][1]){
                            if(deg2[j] == edg[k][0])
                                count++;
                        }
                    }
                    if(count > 0)
                        continue;
                    weight = (rand() % (100 + 1 - 1)) + 1;
                    buff.push_back(deg2[i]);
                    buff.push_back(deg2[j]);
                    buff.push_back(weight);
                    edg.push_back(buff);
                    buff.clear();
                    degrees[deg2[j]-1]--;
                    degrees[deg2[i]-1]--;
                    if(degrees[deg2[j]-1] == 0){
                        deg2[j] = 0;
                    }
                    if(degrees[deg2[i]-1] == 0){
                        deg2[i] = 0;
                        break;
                    }
                }
            }
        }
    }
    saveInstance(number, edg);
}

void saveInstance(int number, vector<vector<int> > edges){
    string fname;
    string newNumber = to_string(number);
    fname = newNumber + ".txt";
    ofstream outfile;
    outfile.open(fname.c_str(), ios::out);
    if(outfile.good()){
        outfile << edges.size() << endl;
        for(int i = 0; i < edges.size(); i++){
            for(int j = 0; j < edges[i].size(); j++){
                outfile << edges[i][j] << " ";
            }
            outfile << "0" << endl;
        }
        outfile.close();
    }
    else
        cout << "Nie udalo sie zapisac pliku." << endl;
}

void loadInstance(int number, vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix){
    string fname;
    string newNumber = to_string(number);
    fname = newNumber + ".txt";
    ifstream file;
    file.open(fname.c_str(), ios::in);
    int edge;
    vector<vector<int> > edges;
    if (file.good()) {
        vector<int> buff;
        int v;
        file >> edge;
        for (int i = 0; i < edge; i++) {
            buff.clear();
            do {
                file >> v;
                if (v != 0)
                    buff.push_back(v);        // zapisywanie listy nastepnikow w grafie
            } while (v != 0);
            edges.push_back(buff);
        }

        file.close();

        int weight;
        buff.clear();
        for(int i = 1; i <= V; i++){
            for(int j = 0; j < edges.size(); j++){
                if(i == edges[j][0]){
                    buff.push_back(edges[j][1]);
                }
                else if(i == edges[j][1]){
                    buff.push_back(edges[j][0]);
                }
            }
            Graph.push_back(buff);
            buff.clear();
        }

        buff.clear();
        for(int i = 0; i < V; i++){
            for(int j = 0; j < V; j++){
                buff.push_back(0);
            }
            WeightMatrix.push_back(buff);
            buff.clear();
        }

        for(int i = 0; i < edges.size(); i++){
            int v1 = edges[i][0];
            int v2 = edges[i][1];
            weight = edges[i][2];
            WeightMatrix[v1-1][v2-1] = weight;
            WeightMatrix[v2-1][v1-1] = weight;
        }

    }
    else
        cout << "Nie udalo sie otworzyc pliku " << fname << endl;
}

void antsFirstIt(vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix, vector<vector<float> > &PheromoneMatrix){
    vector<vector<int> > Paths;
    int numberOfAnts = 100;

    // wypuszczamy 100 mrowek na ekspedycje

    for(int i = 0; i < numberOfAnts; i++){
        bool allVisited = false;
        vector<int> visited;
        for(int j = 0; j < V; j++)
            visited.push_back(0);
        int current = (rand() % (V + 1 - 1)) + 1;
        int inx, next;
        buff.clear();
        //while na sprawdzenie czy wszystkie wierzcholki zostaly odwiedzone co najmniej raz
        while(!allVisited){
            int count = 0;
            buff.push_back(current);
            visited[current-1]++;
            int range = Graph[current-1].size();
            if(range > 1)
                inx = rand() % range;
            else
                inx = 0;

            next = Graph[current-1][inx];
            current = next;

            for(int j = 0; j < visited.size(); j++){
                if(visited[j] == 0)
                    count++;
            }
            if(count == 0) {
                allVisited = true;
                break;
            }
        }
        Paths.push_back(buff);
        buff.clear();
    }

    // po ekspedycji 100 mrowek pora na liczenie mrowczych kilometrow pokonanych w trakcie ich wycieczek

    vector<int> sValues;
    for(int i = 0; i < Paths.size(); i++){
        int sValue = 0;
        int current, prev, weight;
        for(int j = Paths[i].size()-1; j > 0 ; j--){
            int ver1 = Paths[i][j];
            int ver2 = Paths[i][j-1];
            current = WeightMatrix[ver1 - 1][ver2 - 1];
            if(j < Paths[i].size()-1){
                if(current > prev){
                    weight = current*x;
                }
                else
                    weight = current;
            }
            else
                weight = current;
            sValue += weight;
            prev = current;
        }
        sValues.push_back(sValue);
    }

    int maxValue = sValues[0];
    for(int i = 0; i < sValues.size(); i++)
        if(maxValue < sValues[i])
            maxValue = sValues[i];

    int bestAnts = 30;                      // liczba najlepszych mrowek nie moze przekraczac 1/3 liczby wszystkich mrowek, ktore wyruszyly na ekspedycje
    int value;
    vector<vector<int> > topAnts;
    buff.clear();
    while(topAnts.size() < bestAnts) {
        int inx;
        value = maxValue;
        for (int i = 0; i < sValues.size(); i++) {
            if (value > sValues[i] && sValues[i] != 0) {
                value = sValues[i];
                inx = i;
            }
        }
        buff.push_back(inx);
        buff.push_back(value);
        topAnts.push_back(buff);
        sValues[inx] = 0;
        buff.clear();
    }

    itValue = topAnts[0][1];
    itValues.push_back(itValue);
    vector<float> pheromones;
    float pheromone = 0;
    for(int i = 0; i < V; i++)
        pheromones.push_back(pheromone);

    for(int i = 0; i < V; i++)
        PheromoneMatrix.push_back(pheromones);

    for(int i = topAnts.size()-1; i >= 0; i--){
        int inx = topAnts[i][0];
        for(int j = 0; j < Paths[inx].size()-1; j++) {
            int ver1 = Paths[inx][j] - 1;
            int ver2 = Paths[inx][j + 1] - 1;
            pheromone = 1 - (i * bestAnts * 0.001);
            PheromoneMatrix[ver1][ver2] += pheromone;
            PheromoneMatrix[ver2][ver1] += pheromone;
        }
    }
}

void antsIterations(vector<vector<int> > &Graph, vector<vector<int> > &WeightMatrix, vector<vector<float> > &PheromoneMatrix, int iteration){
    vector<vector<int> > Paths;
    int numberOfAnts = 20;
    int probability = 50 * iteration;

    // tym razem wypuszczamy 20 mrowek na ekspedycje

    for(int i = 0; i < numberOfAnts; i++){
        bool allVisited = false;
        vector<int> visited;
        for(int j = 0; j < V; j++)
            visited.push_back(0);

        unsigned int current, inx, next, prev = 0;
        current = (rand() % (V + 1 - 1)) + 1;
        int value = 0;
        buff.clear();
        //while na sprawdzenie czy wszystkie wierzcholki zostaly odwiedzone co najmniej raz
        while(!allVisited) {
            int count = 0;
            buff.push_back(current);
            visited[current - 1]++;
            value = rand() % 1000;
            if (value > probability) {
                int count2 = 0;
                vector<int> unused;
                vector<int> vertices;
                int range = Graph[current - 1].size();
                for(int j = 0; j < range; j++){
                    if(visited[Graph[current-1][j]-1] == 0){
                        count2++;
                        unused.push_back(j);
                    }
                }
                if(count2 == 0) {
                    if (range > 1) {
                        inx = rand() % range;
                        if(prev != 0 && prev == Graph[current - 1][inx]){
                            if(inx != range-1)
                                inx++;
                            else if(inx != 0)
                                inx--;
                        }
                    }
                    else {
                        inx = 0;
                    }
                }
                else{
                    random_shuffle(unused.begin(), unused.end());
                    inx = unused[0];
                }
                next = Graph[current - 1][inx];
                prev = current;
                current = next;

                for (int j = 0; j < visited.size(); j++) {
                    if (visited[j] == 0)
                        count++;
                }
                if (count == 0) {
                    allVisited = true;
                }
            }
            else if (probability > 1000 || value < probability) {
                // jezeli trzeba uzyc macierzy feromonowej
                int phInx;
                unsigned int listSize = Graph[current - 1].size();
                vector<vector<int> > verProb;
                vector<int> buff2;
                if(listSize == 1){
                    phInx = 0;
                }
                else{
                    for (int j = 0; j < Graph[current - 1].size(); j++) {
                        buff2.push_back(0);
                        buff2.push_back(0);
                        verProb.push_back(buff2);
                        buff2.clear();
                    }
                    int sum = 0;
                    for (int j = 0; j < Graph[current - 1].size(); j++) {
                        int ver = Graph[current - 1][j];
                        float pheromone = PheromoneMatrix[current - 1][ver - 1] * 100;
                        int value = static_cast<int>(pheromone);
                        verProb[j][0] = sum;
                        sum += value;
                        verProb[j][1] = sum;
                    }

                    int probability2;
                    vector<int> prob;
                    for(int j = 0; j < sum; j++){
                        prob.push_back(j);
                    }

                    random_shuffle(prob.begin(), prob.end());
                    int random_inx = rand() % sum;
                    probability2 = prob[random_inx];
                    for (int j = 0; j < verProb.size(); j++) {
                        if (probability2 >= verProb[j][0] && probability2 < verProb[j][1]) {
                            phInx = j;
                            break;
                        }
                    }
                }
                next = Graph[current - 1][phInx];
                prev = current;
                current = next;
                for (int j = 0; j < visited.size(); j++) {
                    if (visited[j] == 0)
                        count++;
                }
                if (count == 0) {
                    allVisited = true;
                    break;
                }
            }
        }
        Paths.push_back(buff);
        buff.clear();
    }

    // liczenie dlugosci sciezek
    vector<int> sValues;
    for(int i = 0; i < Paths.size(); i++){
        int sValue = 0;
        int current, prev, weight;
        for(int j = Paths[i].size()-1; j > 0 ; j--){
            int ver1 = Paths[i][j];
            int ver2 = Paths[i][j-1];
            current = WeightMatrix[ver1 - 1][ver2 - 1];
            if(j < Paths[i].size()-1){
                if(current > prev){
                    weight = current*x;
                }
                else
                    weight = current;
            }
            else
                weight = current;
            sValue += weight;
            prev = current;
        }
        sValues.push_back(sValue);
    }

    int maxValue = sValues[0];
    for(int i = 0; i < sValues.size(); i++)
        if(maxValue < sValues[i])
            maxValue = sValues[i];

    int bestAnts = 5;                      // liczba najlepszych mrowek nie moze przekraczac 1/3 liczby wszystkich mrowek, ktore wyruszyly na ekspedycje
    int value;
    vector<vector<int> > topAnts;
    buff.clear();
    while(topAnts.size() < bestAnts) {
        int inx;
        value = maxValue;
        for (int i = 0; i < sValues.size(); i++) {
            if (value > sValues[i] && sValues[i] != 0) {
                value = sValues[i];
                inx = i;
            }
        }
        buff.push_back(inx);
        buff.push_back(value);
        topAnts.push_back(buff);
        sValues[inx] = 0;
        buff.clear();
    }

    itValue = topAnts[0][1];
    itValues.push_back(itValue);

    // uaktualnianie tablicy z feromonami

    for(int i = topAnts.size()-1; i >= 0; i--){
        int inx = topAnts[i][0];
        for(int j = 0; j < Paths[inx].size()-1; j++) {
            int ver1 = Paths[inx][j] - 1;
            int ver2 = Paths[inx][j + 1] - 1;
            float pheromone = 1 - (i * bestAnts * 0.001);
            PheromoneMatrix[ver1][ver2] += pheromone;
            PheromoneMatrix[ver2][ver1] += pheromone;
        }
    }
}

void pheromoneEvaporation(vector<vector<float> > &PheromoneMatrix){
    for(int i = 0; i < PheromoneMatrix.size(); i++){
        for(int j = 0; j < PheromoneMatrix[i].size(); j++){
            if(PheromoneMatrix[i][j] > 0){
                float pheromone = PheromoneMatrix[i][j] * 0.85;
                PheromoneMatrix[i][j] = pheromone;
            }
        }
    }
}

void pheromoneSmoothing(vector<vector<float> > &PheromoneMatrix){
    float delta = 0.2;
    for(int j = 0; j < V; j++){
        bool criticalValue = false;
        for(int i = 0; i < V; i++){
            if(PheromoneMatrix[i][j] > MAX){
                criticalValue = true;
                break;
            }
        }

        if(criticalValue) {
            for (int i = 0; i < V; i++) {
                if(PheromoneMatrix[i][j] > 0){
                    float newValue = PheromoneMatrix[i][j] + delta * (MAX - PheromoneMatrix[i][j]);
                    PheromoneMatrix[i][j] = newValue;
                }
            }
        }
    }
}