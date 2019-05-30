#include <bits/stdc++.h>

using namespace std;

struct Professor{
    string nome;
    int habilitacoes;
};

struct Escola{
    string nome;
    int habilitacao1;
    int habilitacao2;
};


class Grafo{
    private:
        vector<vector<int>> grafo;
        map<int, Professor> professores;
        map<string, int> professoresInv;
        map<int, Escola> escolas;
        map<string, int> escolasInv;
        vector<string> extracao(string &in);
    public:
        Grafo();
        void initGrafo(string nomeArquuivo);
        bool ehBipatido();
        vector<pair<string, string>> matching();
};

Grafo::Grafo(){}

vector<string> Grafo::extracao(string &in){
    vector<string> res;
    string aux = "";
    for(int i = 0; i < in.size(); ++i){
        if(('a'<= in[i] && in[i] <= 'z') ||
          ('A' <= in[i] && in[i] <= 'z') ||
          ('0' <= in[i] && in[i] <= '9')){
              aux += in[i];
        } else if( aux.size() > 0 && (in[i] == ' ' || in[i] == '\n' || in[i] == '\0' || 
          in[i] == '(' || in[i] == ')' || in[i] == ',' ||in[i] == ':')){
            res.push_back(aux);
            aux = "";
        }
    }
    return res;
}

void Grafo::initGrafo(string nomeArquuivo){
    int contador = 0, contadorOrganizador = 0;
    int pai, filho;
    set<pair<int, pair<int, int>>> arestas;
    ifstream input(nomeArquuivo, ifstream::in);
    
    string linha;
    getline(input, linha);
    if(linha[0] == 'P'){
        while (!input.eof()){
            getline(input, linha);
            if(linha[0] == 'E'){
                break;
            }
            auto sm = extracao(linha);
            if(!professoresInv.count(sm[0])){
                professoresInv[sm[0]] = contador;
                professores[contador] = {sm[0], stoi(sm[1])};
                ++contador;
            }
            pai = professoresInv[sm[0]];
            for(int i = 2; i < sm.size(); ++i){
                if(!escolasInv.count(sm[i])){
                    escolasInv[sm[i]] = contador;
                    escolas[contador] = {sm[i], -1, -1};
                    ++contador;
                }

                filho = escolasInv[sm[i]];
                arestas.insert({contadorOrganizador, {pai, filho}});
                ++contadorOrganizador;
            }
        }
    }
    if(linha[0] == 'E'){
        while (!input.eof()){
            getline(input, linha);
            if(linha.size() <= 1){
                continue;
            }
            auto sm = extracao(linha);
            if(sm.size() == 3){
                escolas[escolasInv[sm[0]]] = {sm[0], stoi(sm[1]), stoi(sm[2])};
            } else { // if(sm.size() == 2){
                escolas[escolasInv[sm[0]]] = {sm[0], stoi(sm[1]), -1};
            }
            
        }
    }
    grafo.resize(contador);
    
    for(auto&p: escolas){
        if(p.second.habilitacao2 != -1 && p.second.habilitacao1 > p.second.habilitacao2){
            swap(p.second.habilitacao1, p.second.habilitacao2);
        }
    }
    for(const auto&p: arestas){
        grafo[p.second.first].push_back(p.second.second);
        grafo[p.second.second].push_back(p.second.first);
    }
}

vector<pair<string, string>> Grafo::matching(){
    vector<pair<string, string>> res;
    vector<vector<int>> liberdade(grafo.size()); // se o profesor 
    vector<int> comeca(grafo.size());
    vector<int> numDeAssociados(grafo.size());
    queue<int> fila;
    bool flag;
    for(auto&p: professores){
        fila.push(p.first);
    }
    long long z = 0;
    while(!fila.empty()){
        ++z;
        if(z == 3275923){
            break;
        }
        auto professor = fila.front();
        fila.pop();
        int primEscProf = grafo[professor][comeca[professor]];
        flag = false;
        if(numDeAssociados[primEscProf] == 0){
            liberdade[primEscProf].push_back(professor);
            numDeAssociados[primEscProf]++;
            continue;
        } else if(numDeAssociados[primEscProf] == 1 && escolas[primEscProf].habilitacao2 != -1){
            liberdade[primEscProf].push_back(professor);
            numDeAssociados[primEscProf]++;
            if(liberdade[primEscProf][0] > liberdade[primEscProf][1]){
                if(escolas[primEscProf].habilitacao1 < professores[liberdade[primEscProf][0]].habilitacoes &&
                  escolas[primEscProf].habilitacao1 > professores[liberdade[primEscProf][1]].habilitacoes &&
                  escolas[primEscProf].habilitacao2 < professores[liberdade[primEscProf][1]].habilitacoes){
                      swap(liberdade[primEscProf][0], liberdade[primEscProf][1]);
                  }
            }
            continue;
        }
        
        for(int i = 0; i < numDeAssociados[primEscProf]; ++i){        
            if(i == 0){
                if((professores[liberdade[primEscProf][i]].habilitacoes < escolas[primEscProf].habilitacao1 && 
                  professores[liberdade[primEscProf][i]].habilitacoes < professores[professor].habilitacoes) 
                  || (professores[liberdade[primEscProf][i]].habilitacoes > escolas[primEscProf].habilitacao1 && 
                  professores[liberdade[primEscProf][i]].habilitacoes > professores[professor].habilitacoes &&
                  professores[professor].habilitacoes >= escolas[primEscProf].habilitacao1)
                  ){
                    comeca[liberdade[primEscProf][i]]++;
                    // comeca[liberdade[primEscProf][i]] %= grafo[liberdade[primEscProf][i]].size();
                    if(comeca[liberdade[primEscProf][i]] < grafo[liberdade[primEscProf][i]].size())
                       fila.push(liberdade[primEscProf][i]);
                       
                    liberdade[primEscProf][i] = professor;
                    flag = true;
                    break;
                }
            } else{
                if((professores[liberdade[primEscProf][i]].habilitacoes < escolas[primEscProf].habilitacao2 && 
                  professores[liberdade[primEscProf][i]].habilitacoes < professores[professor].habilitacoes) 
                  || (professores[liberdade[primEscProf][i]].habilitacoes > escolas[primEscProf].habilitacao2 && 
                  professores[liberdade[primEscProf][i]].habilitacoes > professores[professor].habilitacoes &&
                  professores[professor].habilitacoes >= escolas[primEscProf].habilitacao2)
                  ){
                    comeca[liberdade[primEscProf][i]]++;
                    // comeca[liberdade[primEscProf][i]] %= grafo[liberdade[primEscProf][i]].size();
                    if(comeca[liberdade[primEscProf][i]] < grafo[liberdade[primEscProf][i]].size())
                       fila.push(liberdade[primEscProf][i]);
                       
                    liberdade[primEscProf][i] = professor;
                    flag = true;
                    break;
                }
            }
        }
        if(flag){
            if(numDeAssociados[primEscProf] == 2){
                if(liberdade[primEscProf][0] > liberdade[primEscProf][1]){
                    if(escolas[primEscProf].habilitacao1 < professores[liberdade[primEscProf][0]].habilitacoes &&
                    escolas[primEscProf].habilitacao1 > professores[liberdade[primEscProf][1]].habilitacoes &&
                    escolas[primEscProf].habilitacao2 < professores[liberdade[primEscProf][1]].habilitacoes){
                        swap(liberdade[primEscProf][0], liberdade[primEscProf][1]);
                    }
                }
            }
        } else{
            comeca[professor]++;
            // comeca[professor] %= grafo[professor].size();
            if(comeca[professor] < grafo[professor].size())
                fila.push(professor);
        }
    }
    long long auxxxxxxx  =0;
    for(auto &p: professores){
        auxxxxxxx += comeca[p.first];
    }
    for(int i = 0; i < liberdade.size(); ++i){
        if(escolas.count(i))
            if(liberdade[i].size() == 1){
                res.push_back({professores[liberdade[i][0]].nome, escolas[i].nome});
            } else if(liberdade[i].size() == 2){
                res.push_back({professores[liberdade[i][1]].nome, escolas[i].nome});
                res.push_back({professores[liberdade[i][0]].nome, escolas[i].nome});
            }
    }
    return res;
}

int main(){

    Grafo grafo;
    grafo.initGrafo("arquivo.txt");
    auto res = grafo.matching();
    vector<pair<string, vector<string>>> veetor;
    set<string> oiyo;
    for(auto&p : res){
        // cout << p.first << " " << p.second << "\n";
        if(!oiyo.count(p.second)){
            oiyo.insert(p.second);
            veetor.push_back({p.second, vector<string>()});
        }
    }
    sort(veetor.begin(), veetor.end(), [](pair<string, vector<string>> a, pair<string, vector<string>> b){
        return stoi(a.first.substr(1)) < stoi(b.first.substr(1));
    });
    map<string, int> mapa;
    for(int i = 0; i < veetor.size(); ++i){
        auto&p = veetor[i];
        mapa[p.first] = i;
    }
    for(auto&p : res){
        // cout << p.first << " " << p.second << "\n";
        veetor[mapa[p.second]].second.push_back(p.first);
    }

    for(int i = 0; i < veetor.size(); ++i){
        auto&p = veetor[i];
        cout << p.first << ":";
        sort(p.second.begin(), p.second.end(), [](string a, string b){
            return stoi(a.substr(1)) < stoi(b.substr(1));
        });
        for(int j = 0; j < p.second.size(); ++j){
            auto&q = p.second[j];
            cout << " " << q;
        }
        cout << "\n";
    }

    return 0;
}