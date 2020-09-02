#pragma once

#include "TCPServer.h"
#include "ErrorHandler.h"

/// <summary>
/// Executes a command line argument placing the output of the command in the Server parameter and sending the server parameter back to the connector 
/// </summary>
/// <param name="command"></param>
/// <param name="pServerParam"></param>
/// <returns></returns>
BOOL ExecuteCommand(PSERVERPARAM pServerParam);

