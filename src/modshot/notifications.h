#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

struct NotificationsPrivate;
struct RGSSThreadData;

class Notifications
{
public:
	Notifications(RGSSThreadData &threadData);
	~Notifications();

#ifdef _WIN32
	bool addTrayIcon(const char *tip);
	bool delTrayIcon();
	bool hasTrayIcon();
#elif defined __linux__
	bool regApp(const char *appId);
	bool quitApp();
	bool hasGApp();
#endif
	bool send(const char *title, const char *body, const int iconId, const char *iconPath);

private:
	NotificationsPrivate *p;
	RGSSThreadData &threadData;
};

#endif // NOTIFICATIONS_H
