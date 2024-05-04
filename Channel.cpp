#include "Channel.hpp"

Channel::Channel(Client &creator, std::string chname, Server &s)
:name(chname), topicLock(true), hasLimit(false), hasKey(false){ //MM topic is locked by default
    this->operators.push_back(creator); //the channel creator is considered an operator by default
    this->mode = "";//////ik
    s.addChannel(*this);
}

Channel::~Channel(){
    // server->clearChannel(*this);
}

//setters
void Channel::setMode(std::string newMode){
    this->mode = newMode;
}
void Channel::setTopic(std::string newTopic){
    this->topic = newTopic;
}
void Channel::setKey(std::string k){
    this->key = k;
}
// void Channel::setModeLock(bool b){
//     modeLock = b;
// }
void Channel::setTopicLock(bool b){
    this->topicLock = b;
}

void Channel::setHasLimit(bool b){
    this->hasLimit = b;
}
void Channel::setLimit(unsigned int l){
    this->limit = l;
}

void Channel::setHasKey(bool b){
    this->hasKey = b;
}

//getters
std::string Channel::getName() const{
    return this->name;
}
std::string Channel::getTopic() const{
    return this->topic;
}
std::string Channel::getMode() const{
    return this->mode;
}
std::string Channel::getKey() const{
    return this->key;
}
int Channel::getLimit() const{
    return this->limit;
}
// bool Channel::isModelocked() const{
//     return modeLock;
// }
bool Channel::isTopiclocked() const{
    return this->topicLock;
}

bool Channel::getHasLimit(){
    return this->hasLimit;
}
bool Channel::getHasKey(){
    return this->hasKey;
}

void Channel::removeRegularUser(Client & c){
    for (unsigned int i = 0; i < this->regularUsers.size(); i++){
        if (toLowerCase(this->regularUsers[i].getNickname()) == toLowerCase(c.getNickname())){
            this->regularUsers.erase(this->regularUsers.begin() + i);
            break ;
        }
    }
}

void Channel::addOperator(Client & c){
    if (c.isRegistered()){
        if (isRegularuser(c))
            removeRegularUser(c);
        this->operators.push_back(c);
    }
}

void Channel::removeOperator(Client & c){
    for (unsigned int i = 0; i < this->operators.size(); i++){
        if (toLowerCase(this->operators[i].getNickname()) == toLowerCase(c.getNickname())){
            this->operators.erase(this->operators.begin() + i);
            break ;
        }
    }
}

void Channel::addRegularUser(Client &c){
    if (c.isRegistered()){
        if (isOperator(c))
            removeOperator(c);
        this->regularUsers.push_back(c);
    }
}

bool Channel::isOperator(Client const& c){
    for (unsigned int i = 0; i < this->operators.size(); ++i){
        if (toLowerCase(this->operators[i].getNickname()) == toLowerCase(c.getNickname()))
            return true;
    }
    return false;
}
bool Channel::isRegularuser(Client const& c){
    for (unsigned int i = 0; i < this->regularUsers.size(); ++i){
        if (toLowerCase(this->regularUsers[i].getNickname()) == toLowerCase(c.getNickname()))
            return true;
    }
    return false;
}
bool Channel::isMember(Client const& c){
    if (isOperator(c) || isRegularuser(c))
        return true;
    return false;
}
// bool Channel::isfull(){
//     if (this->operators.size() + this->regularUsers.size() == limit)
//         return true;
//     return false;
// }

// void    Channel::addInvited(Client & c){
//     invited.push_back(c);
// }

// bool    Channel::isInvited(Client & c){
//     if (isMember(c)){
//         for (unsigned int i = 0; i < invited.size(); i++)
//             if (invited[i].getNickname() == c.getNickname())
//                 return true;
//     }
//     return false;
// }




//////ik
void Channel::sendMsg2Members(Server &s, Client &c){
    for(size_t i = 0; i < this->regularUsers.size(); ++i){
        if (toLowerCase(this->regularUsers[i].getNickname()) != toLowerCase(c.getNickname()))
            s.sendMsg(this->regularUsers[i].getClientFD(), RPL_JOIN(c.getNickname(), c.getUsername(), this->getName(), this->regularUsers[i].getClientIP()));
    }
    for(size_t i = 0; i < this->operators.size(); ++i){
        if (toLowerCase(this->operators[i].getNickname()) != toLowerCase(c.getNickname()))
            s.sendMsg(this->operators[i].getClientFD(), RPL_JOIN(c.getNickname(), c.getUsername(), this->getName(), this->operators[i].getClientIP()));
    }
}

//AZMARA
void	Channel::sendmsg2chanRegulars(Server S, std::string message){
	for (size_t i = 0; i < this->regularUsers.size(); ++i){
		S.sendMsg(this->regularUsers[i].getClientFD(), message);
		S.sendMsg(this->regularUsers[i].getClientFD(), "\n");
	}
}
void	Channel::sendmsg2chanOperators(Server S, std::string message){
	for (size_t i = 0; i < this->operators.size(); ++i){
		S.sendMsg(this->operators[i].getClientFD(), message);
		S.sendMsg(this->operators[i].getClientFD(), "\n");
	}
}

void Channel::sendNickMsg2All(Server S, std::string message, Client c){
	for (size_t i = 0; i < this->regularUsers.size(); ++i){
        if (toLowerCase(this->regularUsers[i].getNickname()) != toLowerCase(c.getNickname()))
		    S.sendMsg(this->regularUsers[i].getClientFD(), message);
	}
    for (size_t i = 0; i < this->operators.size(); ++i){
        if (toLowerCase(this->operators[i].getNickname()) != toLowerCase(c.getNickname()))
		    S.sendMsg(this->operators[i].getClientFD(), message);
	}
}


////////UPPPPPPPP
bool Channel::hasLimitCanJ(void){
    if((this->operators.size() + this->regularUsers.size()) >= this->limit)
        return (true);
    return (false);
}

std::string Channel::makeStringMember(void){
	std::string member;
	for(size_t i = 0; i < this->operators.size(); ++i){
		if (i != 0)
			member += " ";
		member += "@" + this->operators[i].getNickname();
	}
	for(size_t i = 0; i < this->regularUsers.size(); ++i){
		member += " " + this->regularUsers[i].getNickname();
	}
	return (member);
}

// void Channel::channelStatusMsg(Server &s,std::string modestring, std::string newOp){//MM
//     std::string str = "";
//     std::string sign;
//     if (modestring.find_first_of("+-") == std::string::npos && !modestring.empty())
//         sign = "+";
//     for (unsigned int i = 0; i < modestring.size(); i++){
//         if (modestring[i] == 'k' && this->getHasKey()){
//             str += " ";
//             str += this->getKey();}
//         else if (modestring[i] == 'l' && this->getHasLimit()){
//             std::cout << "limit ==" << getLimit() << "\n";
//             str += " ";
//             std::stringstream ss;
//             ss << this->getLimit();
//             str += ss.str();}
//         else if (modestring[i] == 'o' && !newOp.empty()){
//             str += " ";
//             str += newOp;}
//     }
//     for(unsigned int i = 0; i < this->regularUsers.size(); ++i){
//         std::string msg = ":" + this->regularUsers[i].getNickname() + "!~" + this->regularUsers[i].getUsername() + " " + s.getCommand() + " " + this->getName() + " " + sign + modestring + str + "\n";
//         s.sendMsg(this->regularUsers[i].getClientFD(), msg);
//     }
//     for(unsigned int i = 0; i < this->operators.size(); ++i){
//         std::string msg = ":" + this->operators[i].getNickname() + "!~" + this->operators[i].getUsername() + " " + s.getCommand()+ " " + this->getName() + " " + sign + modestring + str + "\n";
//         s.sendMsg(this->operators[i].getClientFD(), msg);
//     }
// }

std::string Channel::channelModes(){//MM
    std::string str = "";
    if (this->getHasKey())
        str += "k";
    if (this->getHasLimit())
        str += "l";
    if (getMode() == "invite-only")
        str += "i";
    if (this->isTopiclocked())
        str += "t";
    if (!str.empty()){
        for (unsigned int i = 0; str[i] != ' '; i++){
            if (str[i] == 'k' && getHasKey()){//MM
                str += " ";
                str += getKey();
            }
            else if (str[i] == 'l' && getHasLimit()){//MM
                str += " ";
                std::stringstream ss;
                ss << this->getLimit();
                str += ss.str();
            }
        }
    }
    std::cout << "str==" << str << "\n";
    return (str);
}

void	Channel::updateAmemNickName(Client c, std::string newNick){
    for (unsigned int i = 0; i < this->operators.size(); i++){
        if (toLowerCase(this->operators[i].getNickname()) == toLowerCase(c.getNickname())){
            this->operators[i].setNickname(newNick);
            return ;
        }
    }
    for (unsigned int i = 0; i < this->regularUsers.size(); i++){
        if (toLowerCase(this->regularUsers[i].getNickname()) == toLowerCase(c.getNickname())){
            this->regularUsers[i].setNickname(newNick);
            return ;
        }
    }
}

//$$$$$$$$$$
std::string    Channel::toLowerCase(std::string str){
    if (!str.empty()){
        for (unsigned int i = 0; i < str.size(); ++i)
            str[i] = std::tolower(str[i]);
    }
    return (str);
}

void Channel::sendMsgKick2Members(Server &s, Client &c, std::string name){
    for (size_t i = 0; i < this->regularUsers.size(); ++i){
		s.sendMsg(this->regularUsers[i].getClientFD(), RPL_KICK(c.getNickname(), \
        c.getUsername(), c.getHostname(), this->getName(), name));
	}
    for (size_t i = 0; i < this->operators.size(); ++i){
		s.sendMsg(this->operators[i].getClientFD(), RPL_KICK(c.getNickname(), \
        c.getUsername(), c.getHostname(), this->getName(), name));
	}
}