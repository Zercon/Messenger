#include "pch.h"
#include "talk_to_client.h"
#include "Controller.h"

void talk_to_client::answer_to_client()
{
    try
    {
        read_request();
        //process_request();
    }
    catch (boost::system::system_error&)
    {
        stop();
    }
    if (timed_out())
        stop();
}

bool talk_to_client::timed_out() const
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    long long ms = (now - last_ping).total_milliseconds();
    return false;//ms > 500000;
}

void talk_to_client::stop()
{
    boost::system::error_code err; sock_.close(err);
}

void talk_to_client::read_request()
{
    if (sock_.available())
    {
        got_bytes = 0;
        Packet command;
        boost::system::error_code ec;
#pragma pack(push,1)
        struct com {
            uint8_t cod;
            uint32_t length;
            uint16_t ident;
            uint16_t itemCount;
        } hdr;
#pragma pack(pop)
        char mas[9];
        if (got_bytes += boost::asio::read(sock_, boost::asio::buffer(&mas, sizeof(hdr)), boost::asio::transfer_at_least(1), ec)) {
            if (ec) {
                std::cout << "ERROR1: failed to read data: " << ec.message() << std::endl;
                return;
            }
            memcpy(&hdr, mas, 9);
            command.init(hdr.cod, hdr.length, hdr.ident, hdr.itemCount);
            got_bytes += boost::asio::read(sock_, boost::asio::buffer(command.payload(), hdr.length - sizeof(hdr)), boost::asio::transfer_at_least(1), ec);
            if (ec) {
                std::cout << "ERROR2: failed to read data: " << ec.message() << std::endl;
                return;
            }
            for (int i = 0; i < got_bytes; i++) {
                printf("%02X ", *(command.data() + i));
            }
            printf("\n");
            if (got_bytes == hdr.length)
            {
                process_request(std::move(command));
            }
        }
    }
}

void talk_to_client::process_request(Packet&& packet)
{
    last_ping = boost::posix_time::microsec_clock::local_time();
    if ((int)packet.getCod() == 1)
    {
        handle_register(std::move(packet));
    }
    else if ((int)packet.getCod() == 2)
    {
        handle_login(std::move(packet));
    }
    else if ((int)packet.getCod() == 3)
    {
        handle_ping(std::move(packet));
    }
    else if ((int)packet.getCod() == 4)
    {
        handle_request_list(std::move(packet));
    }
    else if ((int)packet.getCod() == 5)
    {
        handle_request_diologe(std::move(packet));
    }
    else if ((int)packet.getCod() ==6)
    {
        handle_message(std::move(packet));
    }
    else
    {
        std::cout << "Error: invalid cod msg!" << std::endl;
    }
}

void talk_to_client::handle_register(Packet && packet)
{
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    std::string pwd((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    //std::string query = "INSERT INTO messeneger.users (login, password) VALUES ('" + login +"', '" + pwd +"');";
    ////std::string query = "SELECT * FROM messeneger.users;";
    //try {
    //    controller_->query((char*)query.c_str(), query.length());
    //}
    //catch (std::exception e) {
    //    error_message(std::move(packet));
    //}
    std::ifstream login_file;
    login_file.open("login.txt");
    if (!login_file.is_open())
    {
        error_message(std::move(packet));
        return;
    }
    std::string tmp_login;
    while (!login_file.eof())
    {
        login_file >> tmp_login;
        if (tmp_login == login)
        {
            error_message(std::move(packet));
            return;
        }
        login_file >> tmp_login;
    }
    login_file.close();
    std::ofstream of;
    of.open("login.txt", std::ofstream::app);
    if (!of.is_open()) {
        error_message(std::move(packet));
        return;
    }
    of << login << " " << pwd << std::endl;
    of.close();
    Packet ans(129,9,packet.getIdent());
    ans.makeCodItem(1);
    write(std::move(ans));
}

void talk_to_client::handle_login(Packet && packet)
{
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    std::string pwd((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    //std::string query = "SELECT * FROM messeneger.users WHERE login = '" + login + "' AND password = '" + pwd + "';";
    ////std::string query = "SELECT * FROM messeneger.users;";
    //try {
    //    if (!controller_->query_login((char*)query.c_str(), query.length())) {
    //        error_message(std::move(packet), 2);
    //    }
    //    else
    //    {
    //        error_message(std::move(packet), 1);
    //        username_ = login;
    //    }
    //}
    //catch (std::exception e) {
    //    error_message(std::move(packet));
    //}
    std::ifstream fin;
    fin.open("login.txt");
    std::string tmp_login, tmp_pwd;
    if (!fin.is_open())
    {
        error_message(std::move(packet));
        return;
    }
    while (!fin.eof())
    {
        fin >> tmp_login >> tmp_pwd;
        if (tmp_login == login && tmp_pwd == pwd)
        {
            username_ = login;
            Packet ans(130, 9, packet.getIdent());
            ans.makeCodItem(1);
            write(std::move(ans));
            break;
        }
        else if(tmp_login == login)
        {
            Packet ans(130, 9, packet.getIdent());
            ans.makeCodItem(2);
            write(std::move(ans));
            fin.close();
            return;
        }
    }
    fin.close();
}

void talk_to_client::handle_ping(Packet && packet)
{
    write(std::move(packet));
}

void talk_to_client::handle_request_list(Packet && packet)
{
    if (username_ == "NONE")
    {
        std::cout << "Declien " << username_ << std::endl;
        error_message(std::move(packet));
        return;
    }
    //std::string query = "SELECT * FROM messeneger.users;";
    //try {
    //    MYSQL_RES * res = controller_->query_list((char*)query.c_str(), query.length());
    //    Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
    //    if (res)
    //    {
    //        
    //        MYSQL_ROW row;
    //        while (row = mysql_fetch_row(res))
    //        {
    //           // for (int i = 0; i < mysql_num_fields(res); i++)
    //            //{
    //            //    std::cout << row[i] << std::endl;
    //            ans.makeStringItem(row[0], sizeof(row[0]) / sizeof(char));
    //            //}
    //        }
    //    }
    //    write(std::move(ans));
    //}
    //catch (std::exception e) {
    //    error_message(std::move(packet));
    //}
    std::ifstream fin;
    fin.open("login.txt");
    if (!fin.is_open())
    {
        error_message(std::move(packet));
        return;
    }
    std::string tmp, buff;
    Packet ans(132, 9, packet.getIdent());
    while (fin.eof())
    {
        fin >> tmp >> buff;
        ans.makeStringItem((const char*)tmp.c_str(), tmp.length());
    }
    write(std::move(ans));
    fin.close();
}

void talk_to_client::handle_request_diologe(Packet && packet)
{
    if (username_ == "NONE")
    {
        std::cout << "Declien " << username_ << std::endl;
        error_message(std::move(packet));
        return;
    }
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    //std::string query = "SELECT * FROM messeneger.messeges WHERE to_user = '" + login + "' AND from_user = '" + pwd + "';";
    ////std::string query = "SELECT * FROM messeneger.users;";
    //try {
    //    MYSQL_RES * res = controller_->query_dial((char*)query.c_str(), query.length());
    //    Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
    //    if (res)
    //    {
    //        ans.makeCodItem(1);
    //        write(std::move(ans));
    //        MYSQL_ROW row;
    //        while (row = mysql_fetch_row(res))
    //        {
    //            Packet anss(packet.getCod() + 128, 9, packet.getIdent(), 0);
    //            anss.makeStringItem(row[2], sizeof(row[2]) / sizeof(char));
    //            anss.makeStringItem(row[4], sizeof(row[4]) / sizeof(char));
    //            write(std::move(packet));
    //        }
    //    }
    //    write(std::move(ans));
    //}
    //catch (std::exception e) {
    //    error_message(std::move(packet));
    //}
    std::ofstream fout;
    std::string name = username_ + login + ".txt";
    fout.open(name);
    if (!fout.is_open())
    {
        return;
    }
    fout.close();
    std::ifstream fin;
    fin.open(name);
    if (!fin.is_open())
    {
        error_message(std::move(packet));
        return;
    }
    std::string login_t, buff;
    Packet ans;
    while (fin.eof())
    {
        std::getline(fin, login_t);
        std::getline(fin, buff);
        ans.makeStringItem((const char *)login_t.c_str(), login_t.length());
        ans.makeStringItem((const char *)buff.c_str(), buff.length());
    }
    write(std::move(ans));
    fin.close();
}

void talk_to_client::handle_message(Packet && packet)
{
    //uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    /*std::string login_to(packet.getStringItem(1), packet.getLengthItem(1));
    std::string login_from(packet.getStringItem(2), packet.getLengthItem(2));
    std::string text(packet.getStringItem(3), packet.getLengthItem(3));*/
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login_to((char*)packet.data() + 9 + 5, len - 5);
    std::string mes((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    //len += *((uint32_t*)(packet.data() + 9 + len + 1));
    //std::string text((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    if (username_ == "NONE")
    {
        std::cout << "Declien " << username_ << " " << username_ << std::endl;
        error_message(std::move(packet));
        return;
    }
    //std::string query = "INSERT INTO `messeneger`.`messeges` (`to_user`, `from_user`, `create_time`, `content`) VALUES ('"+ login_to+"', '"+login_from+"', '10001', '"+text+"');";
    ////std::string query = "SELECT * FROM messeneger.users;";
    //try {
    //    controller_->query((char*)query.c_str(), query.length());
    //    controller_->write_mes_to(std::move(packet));
    //    error_message(std::move(packet),1);
    //}
    //catch (std::exception e) {
    //    error_message(std::move(packet));
    //}
    
    std::ofstream fout;
    std::string name = username_ + login_to + ".txt";
    fout.open(name, std::ofstream::app);
    if (!fout.is_open())
    {
        return;
    }
    fout << username_ << std::endl;
    fout << mes << std::endl;
    fout.close();
    std::ofstream fout2;
    std::string name2 = login_to + username_ + ".txt";
    fout.open(name2, std::ofstream::app);
    if (!fout2.is_open())
    {
        return;
    }
    fout2 << username_ << std::endl;
    fout2 << mes << std::endl;
    fout2.close();
}

void talk_to_client::error_message(Packet && packet, uint8_t cod)
{
    Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
    ans.makeCodItem(cod);
    write(std::move(ans));
}

void talk_to_client::write(Packet && packet) {
    std::string str((char*)packet.data(), packet.getLength());
    boost::asio::write(sock_, boost::asio::buffer(str, str.length()));
}