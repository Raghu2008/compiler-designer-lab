#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <time.h>
using namespace std;

#define get_all_args(v,l) vector<string> v{explode(l, ',')}

#define negate(l) do{\
			while(l.at(0)==' ') l=l.substr(1);\
			if(l.at(0)!='~') l="~"+l;\
			else l=l.substr(1);\
			}while(0)
			
int glob_count=0;
const vector<string> explode(const string& s, const char& c)
{
	string buff{""};
	vector<string> v;
	
	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}

inline int has_tilde(const string& a)
{
	if(a.at(0)=='~')return 1;
	return 0;
}

inline void remove_lead_trail(string& n)
{
	while(n.at(0)==' ') n=n.substr(1);
	while(n.at(n.length()-1)==' ') n=n.substr(0,n.length()-1);
}

inline int var(const string& n)
{
	if(islower(n.at(0))) return 1;
	return 0;
}

void standardize(string& l,int i)
{
	int start=l.find("(");
	int end=l.find(")");
	get_all_args(v,l.substr(start+1,end-start-1));
	int n_o_c=v.size()-1;
	string temp="";
	for(auto n:v)
	{
		if(var(n)) temp.append(n+to_string(i));
		else temp.append(n);
		if(n_o_c)
		{
			temp.append(",");
			n_o_c--;
		}
	}
	l = l.substr(0,start+1) + temp + ")"; 	
}

int unify(string& vq, string& kb_stat, string& f_vq, int loc)
{
	string temp(kb_stat.substr(kb_stat.find(vq.substr(0,vq.find("(")),loc)));
	int start=temp.find("(");
	int end=temp.find(")");
	get_all_args(vq_args,vq.substr(vq.find("(")+1,vq.find(")")-vq.find("(")-1));
	get_all_args(kb_stat_args,temp.substr(start+1,end-start-1));
	unordered_map<string,string> subst;
	unordered_map<string,string> vq_subst;
	std::unordered_map<std::string,string>::const_iterator subst_iter;
	for (int i=0;i<kb_stat_args.size();++i)
	{
		if((var(kb_stat_args[i]) && var(vq_args[i]))) 
		{
			vq_subst.insert({{vq_args[i],vq_args[i].at(0)+to_string(glob_count)}});
			subst.insert({{kb_stat_args[i],vq_args[i].at(0)+to_string(glob_count)}});
			vq_args[i]=vq_args[i].at(0)+to_string(glob_count);
			glob_count++;
		}
		else if((!var(kb_stat_args[i]) && var(vq_args[i]))) 
		{
			vq_subst.insert({{vq_args[i],kb_stat_args[i]}});
			vq_args[i]=kb_stat_args[i];
		}
		else if (var(kb_stat_args[i]) && !var(vq_args[i])) subst.insert({{kb_stat_args[i],vq_args[i]}});
		else if (!var(kb_stat_args[i]) && !var(vq_args[i]) && kb_stat_args[i].compare(vq_args[i])!=0) return 0;
	}

	temp="";
	temp+=vq.substr(0,vq.find("(")+1);
	int num_of_c=vq_args.size()-1;
	for(auto n:vq_args)
	{
		temp+=n;
		if(num_of_c)
		{
			temp+=",";
			num_of_c--;
		}
	}
	temp+=")";
	vq="";
	vq.append(temp);
	temp="";
	vector<string> v{explode(f_vq, '|')};
	int n_o_r=v.size()-1;
	for(auto n:v)
	{
		start=n.find("(");
		end=n.find(")");
		get_all_args(clause_args,n.substr(start+1,end-start-1));
		temp+=n.substr(0,start+1);
		int n_o_c=clause_args.size()-1;
		for(auto k:clause_args)
		{
			subst_iter=vq_subst.find(k);
			if(subst_iter == vq_subst.end()) temp+=k;
			else temp+=subst_iter->second;
			if(n_o_c)
			{
				temp+=",";
				n_o_c--;
			}
		}
		temp+=")";
		if(n_o_r)
		{
			temp+=" |";
			n_o_r--;
		}
	}
	f_vq = "";
	f_vq.append(temp);
	temp="";
	vector<string> z{explode(kb_stat, '|')};
	n_o_r=z.size()-1;
	for(auto n:z)
	{
		start=n.find("(");
		end=n.find(")");
		get_all_args(clause_args,n.substr(start+1,end-start-1));
		temp+=n.substr(0,start+1);
		int n_o_c=clause_args.size()-1;
		for(auto k:clause_args)
		{
			subst_iter=subst.find(k);
			if(subst_iter == subst.end()) temp+=k;
			else temp+=subst_iter->second;
			if(n_o_c)
			{
				temp+=",";
				n_o_c--;
			}
		}
		temp+=")";
		if(n_o_r)
		{
			temp+=" |";
			n_o_r--;
		}
	}
	kb_stat = "";
	kb_stat.append(temp);
	return 1;
}
void resolve(const string& vq, const string& kb_stat, string& f_vq)
{
	string temp_neg(vq);
	negate(temp_neg);
	string temp="";
	vector<string> z{explode(kb_stat, '|')};
	int pos;
	for(int i=0;i<z.size();i++)
	{
		remove_lead_trail(z[i]);
		if(z[i].compare(vq)==0) pos=i;
	}
	z.erase(z.begin()+pos);
	int n_o_r=z.size()-1;
	for(auto n:z)
	{
		temp+=n;
		if(n_o_r)
		{
			temp+=" | ";
			n_o_r--;
		}
	}
	vector<string> q{explode(f_vq, '|')};
	for(int i=0;i<q.size();i++)
	{
		remove_lead_trail(q[i]);
		if(q[i].compare(temp_neg)==0) pos=i;
	}
	q.erase(q.begin()+pos);
	n_o_r=q.size()-1;
	if(q.size()>=1 && z.size()>=1) temp+=" | ";
	for(auto n:q)
	{
		temp+=n;
		if(n_o_r)
		{
			temp+=" | ";
			n_o_r--;
		}
	}
	vector<string> d{explode(temp, '|')};
	unordered_set<string> final_set;
	for(auto n:d)
	{
		remove_lead_trail(n);
		final_set.insert(n);
	}
	n_o_r=final_set.size()-1;
	temp="";
	for(auto n:final_set)
	{
		temp+=n;
		if(n_o_r)
		{	
			temp+=" | ";
			n_o_r--;
		}
	}
	f_vq="";
	f_vq.append(temp);
}

int ask(std::unordered_set<string>& kb,string query)
{
	string temp(query);
	negate(temp);
	vector<string> visited_queries{temp};
	time_t old_secs=time(NULL);
	while (visited_queries.size()>0)
	{
		time_t new_secs=time(NULL);
		if((kb.size() > 15 && (new_secs-old_secs>=35 || new_secs-old_secs>=(int)(kb.size()/2))) || (kb.size() <= 15 && new_secs-old_secs>=7)) break;
		temp="";
		temp.append(visited_queries[0]);
		visited_queries.erase(visited_queries.begin());
		vector<string> v{explode(temp, '|')};
		for(auto n:v)
		{
			remove_lead_trail(n);
			string check(n);
			negate(check);
			string temp2=check.substr(0,check.find("("));
			for (std::unordered_set<string>::iterator it=kb.begin(); it!=kb.end(); ++it)
			{
				vector<size_t> positions; // holds all the positions that sub occurs within str
				size_t pos = (*it).find(temp2, 0);
				while(pos != string::npos)
				{
					positions.push_back(pos);
					pos = (*it).find(temp2,pos+1);
				}
				for(auto loc:positions)
				{
					if((loc==std::string::npos) || (!has_tilde(check) && loc==1) || (loc>0 && !has_tilde(check) && has_tilde((*it).substr(loc-1)))) continue;
					else
					{
						//unify
						string vq(check);
						string f_vq(temp);
						string kb_stat(*it);
						if(unify(vq,kb_stat,f_vq,loc))
						{
							//resolve
							resolve(vq,kb_stat,f_vq);
							if (f_vq.compare("")==0) return 1;
							//insert into visited_queries
							int flag=1;
							for (int i=0; i<visited_queries.size(); ++i)
							{
								if(visited_queries[i].compare(f_vq)==0)
								{
									flag=0;
									break;
								}
							}
							if(flag) visited_queries.push_back(f_vq);
						}
					}
				}
			}
		}
	}
	return 0;
}

int main()
{
	string line;
	ifstream input("input.txt");
	ofstream output("output.txt");
	std::unordered_set<string> kb;
	std::vector<string> queries;
	if (input.is_open())
	{
		int num_of_q,num_of_kb;
		getline(input,line);
		num_of_q=atoi(line.c_str());
		int i=0;
		while(i<num_of_q)
		{
			getline(input,line);
			queries.push_back(line);
			i++;
		}
		getline(input,line);
		num_of_kb=atoi(line.c_str());
		i=0;
		while(i<num_of_kb)
		{
			getline(input,line);
			string temp="";
			vector<string> v{explode(line, '|')};
			int n_o_r=v.size()-1;
			for(auto n:v)
			{
				standardize(n,i);
				temp.append(n);
				if (n_o_r)
				{
					temp.append(" |");
					n_o_r--;
				}
			}
			kb.insert(temp);
			i++;
		}
		for(i=0;i<num_of_q;i++)
		{
			glob_count=num_of_kb;
			if(ask(kb,queries[i])) output<<"TRUE\n";
			else output<<"FALSE\n";
		}
	}
	else cout<<"Error opening \"input.txt\"\n";
	input.close();
	output.close();
}