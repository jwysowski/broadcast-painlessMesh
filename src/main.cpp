/*
 * Rui Santos
 * Complete project details at https://RandomNerdTutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
 *
 * This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
 */

#include "painlessMesh.h"

const char *mesh_prefix = "esp_mesh";
const char *mesh_password = "123456789";
const int mesh_port = 5555;

Scheduler user_scheduler; // to control your personal task
painlessMesh mesh;

//Scheduler
void send_message();
Task task_send_message(TASK_SECOND * 1, TASK_FOREVER, &send_message);

//Mesh callbacks
void received_callback(uint32_t from, String &msg);
void new_connection_callback(uint32_t nodeId);
void changed_connection_callback();
void node_time_adjusted_callback(int32_t offset);

void setup() {
	Serial.begin(9600);

	mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);   // all types on
	// mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

	mesh.init(mesh_prefix, mesh_password, &user_scheduler, mesh_port);
	mesh.onReceive(&received_callback);
	mesh.onNewConnection(&new_connection_callback);
	mesh.onChangedConnections(&changed_connection_callback);
	mesh.onNodeTimeAdjusted(&node_time_adjusted_callback);
	mesh.setContainsRoot(true);

	user_scheduler.addTask(task_send_message);
	task_send_message.enable();
}

void loop() {
	// it will run the user scheduler as well
	mesh.update();
}

void send_message() {
	String msg = "Hi from node 2, id: ";

	msg += mesh.getNodeId();
	mesh.sendBroadcast(msg);
	task_send_message.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

void received_callback(uint32_t from, String &msg) {
	Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void new_connection_callback(uint32_t nodeId) {
	Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changed_connection_callback() {
	Serial.printf("Changed connections\n");
}

void node_time_adjusted_callback(int32_t offset) {
	Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}
