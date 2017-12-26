#include "../Cheat.h"
#include <stdio.h> 
#include <ctime>


CClantagChanger* ClantagChanger;

CClantagChanger::Animation CClantagChanger::Marquee(std::string name, std::string text, int width /*= 15*/)
{
	// Outputs cool scrolling text animation
	char empty = ' ';
	std::replace(text.begin(), text.end(), ' ', empty);

	std::string cropString = std::string(width, empty) + text + std::string(width - 1, empty);

	std::vector<CClantagChanger::Frame> frames;
	for (unsigned long i = 0; i < text.length() + width; i++)
		frames.push_back(CClantagChanger::Frame(cropString.substr(i, width + i), Vars.Misc.ClantagChanger.animation_speed));

	return CClantagChanger::Animation(name, frames, CClantagChanger::ANIM_LOOP);
}

std::vector<std::string> splitWords(std::string text)
{
	std::istringstream stream(text);
	std::string word;
	std::vector<std::string> words;
	while (stream >> word)
		words.push_back(word);

	return words;
}

CClantagChanger::Animation CClantagChanger::Words(std::string name, std::string text)
{
	// Outputs a word by word animation
	std::vector<std::string> words = splitWords(text);
	std::vector<CClantagChanger::Frame> frames;
	for (unsigned long i = 0; i < words.size(); i++)
		frames.push_back(Frame(words[i], Vars.Misc.ClantagChanger.animation_speed));

	return CClantagChanger::Animation(name, frames, CClantagChanger::ANIM_LOOP);
}

CClantagChanger::Animation CClantagChanger::Letters(std::string name, std::string text)
{
	// Outputs a letter incrementing animation
	std::vector<CClantagChanger::Frame> frames;
	for (unsigned long i = 1; i <= text.length(); i++)
		frames.push_back(Frame(text.substr(0, i), Vars.Misc.ClantagChanger.animation_speed));

	for (unsigned long i = text.length() - 2; i > 0; i--)
		frames.push_back(Frame(frames[i].text, Vars.Misc.ClantagChanger.animation_speed));

	return CClantagChanger::Animation(name, frames, CClantagChanger::ANIM_LOOP);
}

void CClantagChanger::UpdateClanTagCallback()
{
	if (strlen(Vars.Misc.ClantagChanger.value) > 0 && Vars.Misc.ClantagChanger.type > STATIC)
	{
		std::string ct = Vars.Misc.ClantagChanger.value;
		switch (Vars.Misc.ClantagChanger.type)
		{
		case MARQUEE:
			*CClantagChanger::animation = CClantagChanger::Marquee(charenc("CUSTOM"), ct);
			break;
		case WORDS:
			*CClantagChanger::animation = CClantagChanger::Words(charenc("CUSTOM"), ct);
			break;
		case LETTERS:
			*CClantagChanger::animation = CClantagChanger::Letters(charenc("CUSTOM"), ct);
			break;
		}

		return;
	}

	CClantagChanger::animations = {
		CClantagChanger::Marquee(charenc("Picoloware"), charenc("Picoloware")),
		CClantagChanger::Words(charenc("Picoloware"), charenc("Picoloware")),
		CClantagChanger::Letters(charenc("Picoloware"), charenc("Picoloware"))
	};

	int current_animation = Vars.Misc.ClantagChanger.type - 1;
	if (current_animation >= 0)
		CClantagChanger::animation = &CClantagChanger::animations[current_animation];
}

void CClantagChanger::BeginFrame()
{
	if (!Vars.Misc.ClantagChanger.enabled)
		return;
	if (!I::Engine->IsInGame())
		return;
	if (!I::Engine->IsConnected())
		return;

	if (Vars.Misc.ClantagChanger.type == CTTIME)
	{
		time_t t = time(0);   // get time now
		struct tm * now = localtime(&t);
		char buffer[30];
		sprintf(buffer, "[%02d:%02d:%02d]", now->tm_hour, now->tm_min, now->tm_sec);

		U::SendClanTag(buffer, "LOCALTIME");
		return;
	}

	if (strlen(Vars.Misc.ClantagChanger.value) == 0 && Vars.Misc.ClantagChanger.type == STATIC)
		return;

	long currentTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	static long timeStamp = currentTime_ms;

	if (currentTime_ms - timeStamp > CClantagChanger::animation->GetCurrentFrame().time)
	{
		timeStamp = currentTime_ms;
		CClantagChanger::animation->NextFrame();
	}

	std::string ctWithEscapesProcessed = Vars.Misc.ClantagChanger.value;
	U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n"); // compute time impact? also, referential so i assume RAII builtin cleans it up...
	U::SendClanTag(Vars.Misc.ClantagChanger.type == STATIC ? ctWithEscapesProcessed.c_str() : CClantagChanger::animation->GetCurrentFrame().text.c_str(), "CLANTAG");
}
