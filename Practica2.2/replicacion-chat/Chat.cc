#include "Chat.h"
#include <assert.h>
#include "utils.h"
#include <string>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
	alloc_data(MESSAGE_SIZE);

	memset(_data, 0, MESSAGE_SIZE);

	//Serializar los campos type, nick y message en el buffer _data
	memcpy(_data, (uint8_t *)&type, sizeof(uint8_t));

	assert(nick.size() <= 8);
	memcpy(_data + sizeof(uint8_t), nick.c_str(), nick.length() + 1);

	assert(message.size() <= 80);
	memcpy(_data + sizeof(uint8_t) + sizeof(char) * 8, message.c_str(), message.length());
}

int ChatMessage::from_bin(char *bobj)
{
	alloc_data(MESSAGE_SIZE);

	memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

	//Reconstruir la clase usando el buffer _data
	memcpy(&type, _data, sizeof(uint8_t));

	nick = std::string(sizeof(char) * 8, '\0');
	memcpy((void *)nick.c_str(), _data + sizeof(uint8_t), sizeof(char) * 8);

	message = std::string(sizeof(char) * 80, '\0');
	memcpy((void *)message.c_str(), _data + sizeof(uint8_t) + (sizeof(char) * 8), sizeof(char) * 80);

	return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
	ChatMessage input;
	while (true)
	{
		/*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
		Socket *other;
		socket.recv(input, other);
		//Recibir Mensajes en y en función del tipo de mensaje
		// - LOGIN: Añadir al vector clients
		// - LOGOUT: Eliminar del vector clients
		// - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
		switch (input.type)
		{
		case ChatMessage::MessageType::LOGIN:
		{
			std::unique_ptr<Socket> sck(other);
			clients.push_back(std::move(sck));
			std::cout << input.nick << " logging in.\n";
			break;
		}
		case ChatMessage::MessageType::LOGOUT:
		{
			auto it = clients.begin();
			bool found =false;
			while (it != clients.end() && !found)
			{
				if(*(*it).get() == *other)
					found = true;
				else
					it++;
			}
			if (it != clients.end())
			{
				clients.erase(it);
				std::cout << input.nick << " logging out.\n";
			}
			else
				printError("Invalid logout client!\n");
			break;
		}
		case ChatMessage::MessageType::MESSAGE:
		{
			auto it = clients.begin();
			while (it != clients.end())
			{
				if (*(*it).get() == *other)
				{
					it++;
					continue;
				}
				socket.send(input, *(*it).get());
				it++;
			}
			std::cout << input.nick << " sending message.\n";
			break;
		}
		default:
		{
			printError("Unsupported message type");
			break;
		}
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
	std::string msg;

	ChatMessage em(nick, msg);
	em.type = ChatMessage::LOGIN;

	socket.send(em, socket);
}

void ChatClient::logout()
{
	// Completar
	ChatMessage em(nick, "");
	em.type = ChatMessage::LOGOUT;
	socket.send(em, socket);
}

void ChatClient::input_thread()
{
	while (true)
	{
		// Leer stdin con std::getline
		// Enviar al servidor usando socket
		ChatMessage msg;
		msg.nick = nick;
		msg.type = ChatMessage::MessageType::MESSAGE;
		std::string in;
		std::getline(std::cin, in);
		if (in == "!q")
		{
			logout();
			break;
		}
		msg.message = in;
		socket.send(msg, socket);
	}
}

void ChatClient::net_thread()
{
	ChatMessage msg;
	while (true)
	{
		//Recibir Mensajes de red
		//Mostrar en pantalla el mensaje de la forma "nick: mensaje"
		Socket *other;
		if (socket.recv(msg, other) == -1)
			logout();
		std::cout << msg.nick << ": " << msg.message << "\n";
	}
}
