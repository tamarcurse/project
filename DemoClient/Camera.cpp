#include "Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
using namespace std;
using namespace std::literals::chrono_literals;
Camera::Camera(char cameraId,Configur &config)
{
	this->cameraId = cameraId;
	//bufferMessege = (baseMessage*)malloc(sizeof(bufferMessege) * N);
	bufferMessege = NULL;
	this->config = config;
	isActive = true;
	bufferString = new Buffer();
	bufferMessegeIndex = -1;
	server.ConnectToMyServer(config.getIp_sever().c_str(),config.getPort_sever());
	//mutexBuffer = new std::mutex();
}

Camera::~Camera()
{
	free(bufferMessege);
	free(bufferString);
}

StatusMesseage* createStatusMessage()
{
	static int id = 0;
	id++;
	return new StatusMesseage(id, 1, rand() % 3 + 1);

}

DiscoverMessege* createDiscoverMessage()
{
	static int id = 99;
	id++;
	return new DiscoverMessege(id, 2, rand() % 9500 + 500, rand() % 361, rand() % 1001);
}

void  Camera::Generate()
{
	baseMessage* baseMessage_;
	int count = rand() % 4 + 1;
	for (int i = 0; i < count; i++)
	{
		std::cout << "genrate messege :";
		++bufferMessegeIndex;
		bufferMessege = (baseMessage**)realloc(bufferMessege, sizeof(baseMessage*) * (bufferMessegeIndex + 1));
		if (bufferMessege == NULL)
			exit(1);
		(rand() % 2 + 1 == 1) ? baseMessage_ = createStatusMessage() : baseMessage_= createDiscoverMessage();
		//mutexBuffer->lock();
		bufferMessege[bufferMessegeIndex] = baseMessage_;
		//mutexBuffer->unlock();
		
		bufferMessege[bufferMessegeIndex]->print();
		std::cout << "\n";
	}


}

void Camera::SendToBUffer()
{
	
		

		std::cout << "move from bufferMesage to Buffer string:\n";
		for (int i = 0; i <= bufferMessegeIndex; i++)
		{
			bufferMessege[i]->parseBack();
			bufferString->AddToBuffer(bufferMessege[i]->getMessageBuffer());
			//bufferMessege[i]->~baseMessage();
			bufferMessege[i]->print();
			std::cout << "\n";
			free(bufferMessege[i]);
		}


		bufferMessegeIndex = -1;
		free(bufferMessege);
		bufferMessege = NULL;
	
}

void Camera::run()
{
	while (isActive)
	{
		Generate();
		SendToBUffer();
		//std::this_thread::sleep_for(3s);
	}
}

void Camera::stop()
{
	isActive = false;
}

void Camera::SendToServer()
{
	while (isActive)
	{
		int x;
		//unsigned char** buffer = bufferString->GetBuffer();
		/*ofstream myfile;
		std::string fileName = "camera" + cameraId;
		myfile.open(fileName+".txt");
		myfile.close();
		while (bufferString->IsEmptay())
		{

		}*/
		//_sleep((60 / num_of_image_to_second) * 1000);
		server.sendMesseToserver(*bufferString, cameraId);
		std::this_thread::sleep_for(3s);
		std::cout << "the buffer sending to sever...\n";
		bufferString->CleanBuffer();

	}
	
}
