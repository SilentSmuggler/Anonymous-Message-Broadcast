# Anonymous Message Broadcast

Anonymous message broadcast is a scheme based on the famous Dining
Cryptographers Problem. The purpose of anonymous message broadcast is to enable
clients to be able to communicate among one another without divulging the identity of
who said what. All the chat participants are, however, aware of who the other
participants are. A useful application of this type of system can be having discussion
between, for example, a manager and her employees, in which none of the employees
wanted the manager to know who said what. Although the manager maybe able to
guess, she will not have a way of conclusively proving it (i.e. by performing some kind
of IP trace).

Implement a system composed of two components: the Server software and the
Client software. The Server software is where the communication is hosted and clients
must connect and authenticate with the server in order to participate in a discussion.
The client software implements the mechanism required for the client to anonymously
communicate with the server. Discussions are formed in groups that are pre-configured
on the server. In order for the discussion to take place all the group members must be
present. The reason this is done will become clear later on in the discussion. The group
is assigned a single password that all members must know in order to be able to
authenticate with the server. A client has to enter their name as well in order to initially
identify himself with other group members so that everyone is aware of who is
currently present in the discussion.
