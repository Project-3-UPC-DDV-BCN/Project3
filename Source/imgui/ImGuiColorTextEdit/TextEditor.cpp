#include <algorithm>
#include <chrono>
#include <string>
#include <regex>

#include "../../ModuleInput.h"
#include "../../Application.h"
#include "../../ModuleTime.h"
#include "TextEditor.h"

static const int cTextStart = 7;
std::map<std::string, std::string> TextEditor::variables;
std::map<std::string, std::string> TextEditor::custom_variables;

// TODO
// - multiline comments vs single-line: latter is blocking start of a ML
// - handle non-monospace fonts
// - handle unicode/utf
// - testing

TextEditor::TextEditor()
	: mLineSpacing(0.0f)
	, mUndoIndex(0)
	, mTabSize(4)
	, mOverwrite(false)
	, mReadOnly(false)
	, mWithinRender(false)
	, mScrollToCursor(false)
	, mWordSelectionMode(false)
	, mColorRangeMin(0)
	, mColorRangeMax(0)
	, mCheckMultilineComments(true)
{
	SetPalette(GetDarkPalette());
	SetLanguageDefinition(LanguageDefinition::HLSL());
	mLines.push_back(Line());
}


TextEditor::~TextEditor()
{
}

void TextEditor::SetLanguageDefinition(const LanguageDefinition & aLanguageDef)
{
	mLanguageDefinition = aLanguageDef;
	mRegexList.clear();

	for (auto& r : mLanguageDefinition.mTokenRegexStrings)
		mRegexList.push_back(std::make_pair(std::regex(r.first, std::regex_constants::optimize), r.second));
}

void TextEditor::SetPalette(const Palette & aValue)
{
	mPalette = aValue;
}

int TextEditor::AppendBuffer(std::string& aBuffer, char chr, int aIndex)
{
	if (chr != '\t')
	{
		aBuffer.push_back(chr);
		return aIndex + 1;
	}
	else
	{
		auto num = mTabSize - aIndex % mTabSize;
		for (int j = num; j > 0; --j)
			aBuffer.push_back(' ');
		return aIndex + num;
	}
}

std::string TextEditor::GetText(const Coordinates & aStart, const Coordinates & aEnd) const
{
	std::string result;

	int prevLineNo = aStart.mLine;
	for (auto it = aStart; it <= aEnd; Advance(it))
	{
		if (prevLineNo != it.mLine && it.mLine < (int)mLines.size())
			result.push_back('\n');

		if (it == aEnd)
			break;

		prevLineNo = it.mLine;
		const auto& line = mLines[it.mLine];
		if (!line.empty() && it.mColumn < (int)line.size())
			result.push_back(line[it.mColumn].mChar);
	}

	return result;
}

TextEditor::Coordinates TextEditor::GetActualCursorCoordinates() const
{
	return SanitizeCoordinates(mState.mCursorPosition);
}

TextEditor::Coordinates TextEditor::SanitizeCoordinates(const Coordinates & aValue) const
{
	auto line = std::max(0, std::min((int)mLines.size() - 1, aValue.mLine));
	auto column = mLines.empty() ? 0 : std::min((int)mLines[line].size(), aValue.mColumn);
	return Coordinates(line, column);
}

void TextEditor::Advance(Coordinates & aCoordinates) const
{
	if (aCoordinates.mLine < (int)mLines.size())
	{
		auto& line = mLines[aCoordinates.mLine];

		if (aCoordinates.mColumn + 1 < (int)line.size())
			++aCoordinates.mColumn;
		else
		{
			++aCoordinates.mLine;
			aCoordinates.mColumn = 0;
		}
	}
}

void TextEditor::DeleteRange(const Coordinates & aStart, const Coordinates & aEnd)
{
	assert(aEnd >= aStart);
	assert(!mReadOnly);

	if (aEnd == aStart)
		return;

	if (aStart.mLine == aEnd.mLine)
	{
		auto& line = mLines[aStart.mLine];
		if (aEnd.mColumn >= (int)line.size())
			line.erase(line.begin() + aStart.mColumn, line.end());
		else
			line.erase(line.begin() + aStart.mColumn, line.begin() + aEnd.mColumn);
	}
	else
	{
		auto& firstLine = mLines[aStart.mLine];
		auto& lastLine = mLines[aEnd.mLine];

		firstLine.erase(firstLine.begin() + aStart.mColumn, firstLine.end());
		lastLine.erase(lastLine.begin(), lastLine.begin() + aEnd.mColumn);

		if (aStart.mLine < aEnd.mLine)
			firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

		if (aStart.mLine < aEnd.mLine)
			RemoveLine(aStart.mLine + 1, aEnd.mLine + 1);
	}
}

int TextEditor::InsertTextAt(Coordinates& /* inout */ aWhere, const char * aValue)
{
	assert(!mReadOnly);

	int totalLines = 0;
	auto chr = *aValue;
	while (chr != '\0')
	{
		if (mLines.empty())
			mLines.push_back(Line());

		if (chr == '\r')
		{
			// skip
		}
		else if (chr == '\n')
		{
			if (aWhere.mColumn < (int)mLines[aWhere.mLine].size())
			{
				auto& newLine = InsertLine(aWhere.mLine + 1);
				auto& line = mLines[aWhere.mLine];
				newLine.insert(newLine.begin(), line.begin() + aWhere.mColumn, line.end());
				line.erase(line.begin() + aWhere.mColumn, line.end());
			}
			else
			{
				InsertLine(aWhere.mLine + 1);
			}
			++aWhere.mLine;
			aWhere.mColumn = 0;
			++totalLines;
		}
		else
		{
			auto& line = mLines[aWhere.mLine];
			line.insert(line.begin() + aWhere.mColumn, Glyph(chr, PaletteIndex::Default));
			++aWhere.mColumn;
		}
		chr = *(++aValue);
	}

	return totalLines;
}

void TextEditor::AddUndo(UndoRecord& aValue)
{
	assert(!mReadOnly);

	mUndoBuffer.resize(mUndoIndex + 1);
	mUndoBuffer.back() = aValue;
	++mUndoIndex;
}

TextEditor::Coordinates TextEditor::ScreenPosToCoordinates(const ImVec2& aPosition) const
{
	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 local(aPosition.x - origin.x, aPosition.y - origin.y);

	int lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));
	int columnCoord = std::max(0, (int)floor(local.x / mCharAdvance.x) - cTextStart);

	int column = 0;
	if (lineNo >= 0 && lineNo < (int)mLines.size())
	{
		auto& line = mLines[lineNo];
		auto distance = 0;
		while (distance < columnCoord && column < (int)line.size())
		{
			if (line[column].mChar == '\t')
				distance = (distance / mTabSize) * mTabSize + mTabSize;
			else
				++distance;
			++column;
		}
	}
	return Coordinates(lineNo, column);
}

ImVec2 TextEditor::CoordinatesToScreenPos(const Coordinates coord) const
{
	auto& line = mLines[coord.mLine];
	int char_num = 0;
	for (int i = 0; i < coord.mColumn; i++)
	{
		if (line[i].mChar == '\t')
		{
			char_num += 4;
		}
		else
		{
			char_num += 1;
		}
	}
	char_num += cTextStart;

	float x = char_num * mCharAdvance.x;
	float y = coord.mLine * mCharAdvance.y;

	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 ret;
	ret.x = x + origin.x;
	ret.y = y + origin.y;

	return ret;
}

TextEditor::Coordinates TextEditor::FindWordStart(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return at;

	auto cstart = (PaletteIndex)line[at.mColumn].mColorIndex;
	while (at.mColumn > 0)
	{
		if (cstart != (PaletteIndex)line[at.mColumn - 1].mColorIndex)
			break;
		--at.mColumn;
	}
	return at;
}

TextEditor::Coordinates TextEditor::FindWordEnd(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return at;

	auto cstart = (PaletteIndex)line[at.mColumn].mColorIndex;
	while (at.mColumn < (int)line.size())
	{
		if (cstart != (PaletteIndex)line[at.mColumn].mColorIndex)
			break;
		++at.mColumn;
	}
	return at;
}

TextEditor::Coordinates TextEditor::FindFullWordStart(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return at;

	bool inside_parenthesis = false;

	while (at.mColumn > 0)
	{
		if (line[at.mColumn - 1].mChar == ')')
		{
			inside_parenthesis = true;
		}
		if (inside_parenthesis)
		{
			if (line[at.mColumn - 1].mChar == '(')
			{
				inside_parenthesis = false;
			}
		}
		if (!inside_parenthesis)
		{
			if (line[at.mColumn - 1].mChar == ' ' || line[at.mColumn - 1].mChar == '\t' || line[at.mColumn - 1].mChar == '.')
				break;
		}
		--at.mColumn;
	}
	return at;
}

bool TextEditor::IsOnWordBoundary(const Coordinates & aAt) const
{
	if (aAt.mLine >= (int)mLines.size() || aAt.mColumn == 0)
		return true;

	auto& line = mLines[aAt.mLine];
	if (aAt.mColumn >= (int)line.size())
		return true;

	return line[aAt.mColumn].mColorIndex != line[aAt.mColumn - 1].mColorIndex;
}

void TextEditor::RemoveLine(int aStart, int aEnd)
{
	assert(!mReadOnly);

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first >= aStart ? i.first - 1 : i.first, i.second);
		if (e.first >= aStart && e.first <= aEnd)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp.insert(i >= aStart ? i - 1 : i);
	}
	mBreakpoints = std::move(btmp);

	mLines.erase(mLines.begin() + aStart, mLines.begin() + aEnd);
}

void TextEditor::RemoveLine(int aIndex)
{
	assert(!mReadOnly);

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first >= aIndex ? i.first - 1 : i.first, i.second);
		if (e.first == aIndex)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i == aIndex)
			continue;
		btmp.insert(i >= aIndex ? i - 1 : i);
	}
	mBreakpoints = std::move(btmp);

	mLines.erase(mLines.begin() + aIndex);
}

TextEditor::Line& TextEditor::InsertLine(int aIndex)
{
	assert(!mReadOnly);

	auto& result = *mLines.insert(mLines.begin() + aIndex, Line());

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
		etmp.insert(ErrorMarkers::value_type(i.first >= aIndex ? i.first + 1 : i.first, i.second));
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
		btmp.insert(i >= aIndex ? i + 1 : i);
	mBreakpoints = std::move(btmp);

	return result;
}

std::string TextEditor::GetWordUnderCursor() const
{
	auto c = GetCursorPosition();
	return GetWordAt(c);
}

std::string TextEditor::GetWordAt(const Coordinates & aCoords) const
{
	auto start = FindWordStart(aCoords);
	auto end = FindWordEnd(aCoords);

	std::string r;

	for (auto it = start; it < end; Advance(it))
		r.push_back(mLines[it.mLine][it.mColumn].mChar);

	return r;
}

std::string TextEditor::GetFullWordAt(const Coordinates & aCoords) const
{
	Coordinates at = aCoords;

	std::string r;

	if (at.mLine >= (int)mLines.size())
		return r;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return r;

	Coordinates new_at = FindFullWordStart(at);

	while (at.mColumn < line.size())
	{
		if (line[at.mColumn].mChar == '(' || line[at.mColumn].mChar == ' ' || line[at.mColumn].mChar == '\t' || line[at.mColumn].mChar == '.' || line[at.mColumn].mChar == '\0')
			break;
		r.push_back(line[at.mColumn].mChar);
		++at.mColumn;
	}

	return r;
}

void TextEditor::FillAutoWord(Char aChar)
{
	if (aChar == ' ' || aChar == '\n' || aChar == '\t')
	{
		is_method_auto_complete_open = false;
		is_word_auto_complete_open = false;
		auto_complete_word.clear();
		auto_complete_word_methods.clear();
		auto_complete_word_list.clear();
		auto_complete_word_list_secondary.clear();
		return;
	}
	static TextEditor::LanguageDefinition def = GetLanguageDefinition();

	if (is_method_auto_complete_open)
	{
		is_word_auto_complete_open = false;
		if (aChar != '\b')
		{
			auto_complete_word_methods.push_back(aChar);
		}
		else
		{
			if (auto_complete_word_methods.empty())
			{
				Coordinates cursor_pos = GetActualCursorCoordinates();
				auto line = mLines[cursor_pos.mLine];
				if (line[cursor_pos.mColumn - 1].mChar != '.')
				{
					is_method_auto_complete_open = false;
				}
				return;
			}
			//is_method_auto_complete_open = false;
			//if (auto_complete_word_methods.empty())
			//{
			//	Coordinates cursor_pos = GetActualCursorCoordinates();
			//	auto line = mLines[cursor_pos.mLine];
			//	/*if (line[cursor_pos.mColumn].mChar == '.')
			//	{
			//		line.pop_back();
			//		return;
			//	}*/
			//	int check_word_start = 0;
			//	if (line[cursor_pos.mColumn - 1].mChar == ')')
			//	{
			//		check_word_start = 3;
			//	}
			//	else
			//	{
			//		check_word_start = 1;
			//	}
			//	std::string new_word = GetWordAt(Coordinates(cursor_pos.mLine, cursor_pos.mColumn - check_word_start));
			//	if (!new_word.empty())
			//	{
			//		bool found = false;
			//		for (std::map<std::string, std::string>::iterator it = def.class_static_auto_complete.begin(); it != def.class_static_auto_complete.end(); it++)
			//		{
			//			if (it->second.find(new_word) != std::string::npos)
			//			{
			//				auto_complete_word = it->first;
			//				is_method_auto_complete_open = false;
			//				is_word_auto_complete_open = false;
			//				found = true;
			//				break;
			//			}
			//		}
			//		if(!found)
			//		{
			//			for (std::map<std::string, std::string>::iterator it = def.class_non_static_auto_complete.begin(); it != def.class_non_static_auto_complete.end(); it++)
			//			{
			//				if (it->second.find(new_word) != std::string::npos)
			//				{
			//					auto_complete_word = it->first;
			//					is_method_auto_complete_open = false;
			//					is_word_auto_complete_open = false;
			//					break;
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}
	if (is_word_auto_complete_open)
	{
		if (aChar != '\b')
		{
			auto_complete_word.push_back(aChar);
		}
		else
		{
			/*is_word_auto_complete_open = false;
			return;*/
			if (auto_complete_word.empty())
			{
				is_word_auto_complete_open = false;
				return;
			}
		}
		auto_complete_index = 0;
		if (!auto_complete_word_list.empty() && aChar != '\b')
		{
			for (std::vector<std::string>::iterator it = auto_complete_word_list.begin(); it != auto_complete_word_list.end(); it++)
			{
				if (it->find(auto_complete_word) != std::string::npos)
				{
					auto_complete_word_list_secondary.push_back(*it);
				}
			}
			auto_complete_word_list.clear();
		}
		else if (!auto_complete_word_list_secondary.empty() && aChar != '\b')
		{
			for (std::vector<std::string>::iterator it = auto_complete_word_list_secondary.begin(); it != auto_complete_word_list_secondary.end(); it++)
			{
				if (it->find(auto_complete_word) != std::string::npos)
				{
					auto_complete_word_list.push_back(*it);
				}
			}
			auto_complete_word_list_secondary.clear();
		}
		else
		{
			auto_complete_word_list.clear();
			auto_complete_word_list_secondary.clear();
			for (std::unordered_set<std::string>::iterator it = def.mClasses.begin(); it != def.mClasses.end(); it++)
			{
				if (it->find(auto_complete_word) != std::string::npos)
				{
					auto_complete_word_list.push_back(*it);
				}
			}
			for (std::map<std::string, std::string>::iterator it = custom_variables.begin(); it != custom_variables.end(); it++)
			{
				if (it->first.find(auto_complete_word) != std::string::npos)
				{
					auto_complete_word_list.push_back(it->first);
				}
			}
		}

		if (auto_complete_word_list.empty() && auto_complete_word_list_secondary.empty())
		{
			is_word_auto_complete_open = false;
		}
	}
}

std::string TextEditor::GetReturnTypeRecursively(std::string word, const Coordinates& aCoords)
{
	std::string ret;

	if (word == "Normalized")
	{
		std::string type = GetWordAt(Coordinates(aCoords.mLine, aCoords.mColumn - 2));
		if (variables.find(type) != variables.end())
		{
			if (variables[type] == "TheQuaternion")
			{
				ret = "TheQuaternion";
			}
			else if (variables[type] == "TheVector3")
			{
				ret = "TheVector3";
			}
		}
		else
		{
			Coordinates c = FindFullWordStart(Coordinates(aCoords.mLine, aCoords.mColumn - 2));
			ret = GetReturnTypeRecursively(type, c);
		}
	}
	return ret;
}

void TextEditor::Render(const char* aTitle, const ImVec2& aSize, bool aBorder)
{
	mWithinRender = true;

	ImGuiIO& io = ImGui::GetIO();
	auto xadv = (io.Fonts->Fonts[1]->IndexXAdvance['X']);
	mCharAdvance = ImVec2(xadv, io.Fonts->Fonts[1]->FontSize + mLineSpacing);

	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImGui::ColorConvertU32ToFloat4(mPalette[(int)PaletteIndex::Background]));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild(aTitle, aSize, aBorder, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

	ImGui::PushAllowKeyboardFocus(true);

	auto shift = io.KeyShift;
	auto ctrl = io.KeyCtrl;
	auto alt = io.KeyAlt;

	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsWindowHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			{
				Copy();
			}

			if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			{
				Paste();
			}

			if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
			{
				Cut();
			}

			if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
			{
				Undo();
			}

			if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
			{
				Redo();
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
			{
				SetSelection(Coordinates(),Coordinates(GetTotalLines(), 0));
			}

			if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_DOWN)
			{
				MoveTop(shift);
			}

			if (App->input->GetKey(SDL_SCANCODE_END) == KEY_DOWN)
			{
				MoveBottom(shift);
			}
		}
		//Delete + Backspace
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			Delete();
		}
		if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
		{
			BackSpace();
		}
		//
		//Arrows
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		{
			if (is_method_auto_complete_open || is_word_auto_complete_open)
			{
				is_method_auto_complete_open = false;
				is_word_auto_complete_open = false;
			}
			MoveRight();
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		{
			if (is_method_auto_complete_open || is_word_auto_complete_open)
			{
				is_method_auto_complete_open = false;
				is_word_auto_complete_open = false;
			}
			MoveLeft();
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			if (is_method_auto_complete_open || is_word_auto_complete_open)
			{
				if (auto_complete_index > 0)
				{
					auto_complete_index--;
				}
			}
			else
			{
				MoveUp();
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			if (is_method_auto_complete_open || is_word_auto_complete_open)
			{
				if (auto_complete_index >= 0)
				{
					auto_complete_index++;
				}
			}
			else
			{
				MoveDown();
			}
		}
		//Enter
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			if (!is_method_auto_complete_open && !is_word_auto_complete_open)
			{
				EnterCharacter('\n');
				auto_complete_word.clear();
			}
		}
		//
		//Page,home,etc...
		if (App->input->GetKey(SDL_SCANCODE_PAGEDOWN) == KEY_DOWN)
		{
			MoveDown(GetPageSize() - 4, shift);
		}
		if (App->input->GetKey(SDL_SCANCODE_PAGEUP) == KEY_DOWN)
		{
			MoveUp(GetPageSize() - 4, shift);
		}

		if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_DOWN)
		{
			MoveHome(shift);
		}
		if (App->input->GetKey(SDL_SCANCODE_END) == KEY_DOWN)
		{
			MoveEnd(shift);
		}
		//
		if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		{
			EnterCharacter('\t');
		}
		if (!IsReadOnly())
		{
			for (size_t i = 0; i < sizeof(io.InputCharacters) / sizeof(io.InputCharacters[0]); i++)
			{
				auto c = (unsigned char)io.InputCharacters[i];
				if (c != 0)
				{
					if (isprint(c) || isspace(c))
					{
						if (c == '\r')
							c = '\n';
						EnterCharacter((char)c);
						if (c == '[')
						{
							Coordinates coord = GetActualCursorCoordinates();
							EnterCharacter(']');
							mState.mCursorPosition = Coordinates(coord.mLine, coord.mColumn);
						}
						if (c == '{')
						{
							Coordinates coord = GetActualCursorCoordinates();
							EnterCharacter('}');
							mState.mCursorPosition = Coordinates(coord.mLine, coord.mColumn);
						}
						if (c == '"')
						{
							Coordinates coord = GetActualCursorCoordinates();
							EnterCharacter('"');
							mState.mCursorPosition = Coordinates(coord.mLine, coord.mColumn);
						}
						if (c == '.')
						{
							auto_complete_is_quaternion = false;
							auto_complete_is_vector = false;

							Coordinates coord = GetActualCursorCoordinates();
							auto line = mLines[coord.mLine];
							Coordinates c = FindFullWordStart(Coordinates(coord.mLine, coord.mColumn - 1));
							std::string word = GetFullWordAt(c);
							
							static TextEditor::LanguageDefinition def = GetLanguageDefinition();
							if (def.mClasses.find(word) != def.mClasses.end())
							{
								auto_complete_word = word;
								is_auto_complete_static = true;
								is_method_auto_complete_open = true;

								if (word == "TheQuaternion")
								{
									auto_complete_is_quaternion = true;
								}
								else if (word == "TheVector3")
								{
									auto_complete_is_vector = true;
								}
							}
							else
							{
								if (line[coord.mColumn - 2].mChar == ')')
								{
									word += "()";
								}

								if (word == "Normalize()" || word == "Lerp()" || word == "Slerp()")
								{
									std::string type = GetWordAt(Coordinates(c.mLine, c.mColumn - 2));
									if (type == "TheQuaternion")
									{
										word += "_TheQuaternion";
										auto_complete_is_quaternion = true;
									}
									else if (type == "TheVector3")
									{
										word += "_TheVector3";
										auto_complete_is_vector = true;
									}
								}

								if (word == "Normalized")
								{
									std::string type = GetReturnTypeRecursively(word, c);
									if (type == "TheQuaternion")
									{
										word += "_TheQuaternion";
										auto_complete_is_quaternion = true;
									}
									else if (type == "TheVector3")
									{
										word += "_TheVector3";
										auto_complete_is_vector = true;
									}
								}

								if (variables.find(word) != variables.end())
								{
									auto_complete_word.clear();
									auto_complete_word = variables[word];
									is_auto_complete_static = false;
									is_method_auto_complete_open = true;
								}
							}
						}
					}
				}
			}
		}
	}

	if (ImGui::IsWindowHovered())
	{
		if (!shift && !alt)
		{
			if (ImGui::IsMouseClicked(0))
			{
				mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
				if (ctrl)
					mWordSelectionMode = true;
				SetSelection(mInteractiveStart, mInteractiveEnd, mWordSelectionMode);
			}
			if (ImGui::IsMouseDoubleClicked(0) && !ctrl)
			{
				mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
				mWordSelectionMode = true;
				SetSelection(mInteractiveStart, mInteractiveEnd, mWordSelectionMode);
			}
			else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
			{
				mState.mCursorPosition = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
				SetSelection(mInteractiveStart, mInteractiveEnd, mWordSelectionMode);
			}
		}

		if (!ImGui::IsMouseDown(0))
			mWordSelectionMode = false;

		if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))
		{
			is_method_auto_complete_open = false;
			is_word_auto_complete_open = false;
			auto_complete_word.clear();
			auto_complete_index = 0;
		}
	}

	ColorizeInternal();

	static std::string buffer;
	auto contentSize = ImGui::GetWindowContentRegionMax();
	auto drawList = ImGui::GetWindowDrawList();
	int appendIndex = 0;
	int longest = cTextStart;

	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	auto scrollX = ImGui::GetScrollX();
	auto scrollY = ImGui::GetScrollY();

	auto lineNo = (int)floor(scrollY / mCharAdvance.y);
	auto lineMax = std::max(0, std::min((int)mLines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));
	if (!mLines.empty())
	{
		if (is_method_auto_complete_open)
		{
			std::map<std::string, std::string> auto_complete;
			if (is_auto_complete_static)
			{
				auto_complete = GetLanguageDefinition().class_static_auto_complete;
			}
			else
			{
				auto_complete = GetLanguageDefinition().class_non_static_auto_complete;
			}
			std::string words = auto_complete[auto_complete_word];
			Coordinates cords = GetActualCursorCoordinates();
			ImVec2 cursor_pos = CoordinatesToScreenPos(cords);
			ImGui::SetNextWindowPos(cursor_pos);

			ImGui::BeginTooltip();
			int word_index = 0;
			for (int i = 0; i < words.size(); i++)
			{
				std::string word;
				while (words[i] != '_' && words[i] != '\0')
				{
					word += words[i];
					i++;
				}
				if (word.find(auto_complete_word_methods) == std::string::npos) continue;
				bool selected = false;
				if (auto_complete_index == word_index)
				{
					selected = true;
				}
				if (ImGui::Selectable(word.c_str(), &selected, ImGuiSelectableFlags_AllowEnterKey))
				{
					size_t size = auto_complete_word_methods.size();
					if (size != 0)
					{
						word.erase(word.begin(), word.begin() + size);
					}
					ImGui::SetClipboardText(word.c_str());
					Paste();
					is_method_auto_complete_open = false;
					auto_complete_index = 0;
					break;
				}
				if (selected || ImGui::IsItemHovered())
				{
					ImVec2 new_tooltip_pos = cursor_pos;
					new_tooltip_pos.x += ImGui::GetWindowSize().x;
					new_tooltip_pos.y = cursor_pos.y;

					ImGui::SetNextWindowPos(new_tooltip_pos);
					ImGui::Begin("methods##auto_complete_methods", &is_method_auto_complete_open, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
					static TextEditor::LanguageDefinition def = GetLanguageDefinition();
					if (word == "Lerp()" || word == "AngleBetween()" || word == "DotProduct()" || word == "Magnitude()" || word == "Normalize()" ||
						word == "Slerp()" || word == "ToAngleAxis()" || word == "Length" || word == "LengthSquared" || word == "Normalized" || word == "Set()" ||
						word == "Scale()")
					{
						if (auto_complete_is_vector)
						{
							word += "_Vec";
							if (is_auto_complete_static)
							{
								word += "_Static";
							}
							else
							{
								word += "_NStatic";
							}
						}
						else if (auto_complete_is_quaternion)
						{
							word += "_Quat";
							if (is_auto_complete_static)
							{
								word += "_Static";
							}
							else
							{
								word += "_NStatic";
							}
						}
						else
						{
							word += "_Math";
						}
					}
					ImGui::Text("Info:");
					ImGui::Separator();
					ImGui::Spacing(5);
					ImGui::Text(def.functions_info[word].c_str());
					ImGui::End();
				}
				word_index++;
			}
			if (auto_complete_index >= word_index)
			{
				auto_complete_index = word_index - 1;
			}
			ImGui::EndTooltip();

			ImGui::SetWindowFocus();
		}

		if (is_word_auto_complete_open)
		{
			Coordinates cords = GetActualCursorCoordinates();
			ImVec2 cursor_pos = CoordinatesToScreenPos(cords);
			cursor_pos.x += 5;
			ImGui::SetNextWindowPos(cursor_pos);

			ImGui::BeginTooltip();
			if (!auto_complete_word_list.empty())
			{
				int i = 0;
				for (std::vector<std::string>::iterator it = auto_complete_word_list.begin(); it != auto_complete_word_list.end(); it++)
				{
					bool selected = false;
					if (auto_complete_index >= auto_complete_word_list.size())
					{
						auto_complete_index = auto_complete_word_list.size() - 1;
					}
					if (i == auto_complete_index)
					{
						selected = true;
					}
					if (ImGui::Selectable((*it).c_str(), &selected, ImGuiSelectableFlags_AllowEnterKey))
					{
						size_t size = auto_complete_word.size();
						if (size != 0)
						{
							(*it).erase((*it).begin(), (*it).begin() + size);
						}
						ImGui::SetClipboardText((*it).c_str());
						Paste();
						is_word_auto_complete_open = false;
						auto_complete_word.clear();
						auto_complete_index = 0;
						break;
					}
					i++;
				}
			}
			else if (!auto_complete_word_list_secondary.empty())
			{
				int i = 0;
				for (std::vector<std::string>::iterator it = auto_complete_word_list_secondary.begin(); it != auto_complete_word_list_secondary.end(); it++)
				{
					bool selected = false;
					if (auto_complete_index >= auto_complete_word_list_secondary.size())
					{
						auto_complete_index = auto_complete_word_list_secondary.size() - 1;
					}
					if (i == auto_complete_index)
					{
						selected = true;
					}
					if (ImGui::Selectable((*it).c_str(), &selected, ImGuiSelectableFlags_AllowEnterKey))
					{
						size_t size = auto_complete_word.size();
						if (size != 0)
						{
							(*it).erase((*it).begin(), (*it).begin() + size);
						}
						ImGui::SetClipboardText((*it).c_str());
						Paste();
						is_word_auto_complete_open = false;
						auto_complete_word.clear();
						auto_complete_index = 0;
						break;
					}
					i++;
				}
			}
			ImGui::EndTooltip();

			ImGui::SetWindowFocus();
		}

		while (lineNo <= lineMax)
		{
			ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * mCharAdvance.y);
			ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mCharAdvance.x * cTextStart, lineStartScreenPos.y);

			auto& line = mLines[lineNo];
			longest = std::max(cTextStart + TextDistanceToLineStart(Coordinates(lineNo, (int)line.size())), longest);
			auto columnNo = 0;
			Coordinates lineStartCoord(lineNo, 0);
			Coordinates lineEndCoord(lineNo, (int)line.size());

			int sstart = -1;
			int ssend = -1;

			assert(mState.mSelectionStart <= mState.mSelectionEnd);
			if (mState.mSelectionStart <= lineEndCoord)
				sstart = mState.mSelectionStart > lineStartCoord ? TextDistanceToLineStart(mState.mSelectionStart) : 0;
			if (mState.mSelectionEnd > lineStartCoord)
				ssend = TextDistanceToLineStart(mState.mSelectionEnd < lineEndCoord ? mState.mSelectionEnd : lineEndCoord);

			if (mState.mSelectionEnd.mLine > lineNo)
				++ssend;

			if (sstart != -1 && ssend != -1 && sstart < ssend)
			{
				ImVec2 vstart(lineStartScreenPos.x + (mCharAdvance.x) * (sstart + cTextStart), lineStartScreenPos.y);
				ImVec2 vend(lineStartScreenPos.x + (mCharAdvance.x) * (ssend + cTextStart), lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::Selection]);
			}

			static char buf[16];

			if (mBreakpoints.find(lineNo + 1) != mBreakpoints.end())
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(lineStartScreenPos, end, mPalette[(int)PaletteIndex::Breakpoint]);
			}

			auto errorIt = mErrorMarkers.find(lineNo + 1);
			if (errorIt != mErrorMarkers.end())
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(lineStartScreenPos, end, mPalette[(int)PaletteIndex::ErrorMarker]);

				if (ImGui::IsMouseHoveringRect(lineStartScreenPos, end))
				{
					ImGui::BeginTooltip();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Error at line %d:", errorIt->first);
					ImGui::PopStyleColor();
					ImGui::Separator();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
					ImGui::Text("%s", errorIt->second.c_str());
					ImGui::PopStyleColor();
					ImGui::EndTooltip();
				}
			}

			snprintf(buf, 16, "%6d", lineNo + 1);
			drawList->AddText(ImVec2(lineStartScreenPos.x /*+ mCharAdvance.x * 1*/, lineStartScreenPos.y), mPalette[(int)PaletteIndex::LineNumber], buf);

			if (mState.mCursorPosition.mLine == lineNo)
			{
				auto focused = ImGui::IsWindowFocused();

				if (!HasSelection())
				{
					auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);
					auto end = ImVec2(start.x + contentSize.x + scrollX, start.y + mCharAdvance.y);
					drawList->AddRectFilled(start, end, mPalette[(int)(focused ? PaletteIndex::CurrentLineFill : PaletteIndex::CurrentLineFillInactive)]);
					drawList->AddRect(start, end, mPalette[(int)PaletteIndex::CurrentLineEdge], 1.0f);
				}

				int cx = TextDistanceToLineStart(mState.mCursorPosition);

				if (focused)
				{
					static auto timeStart = std::chrono::system_clock::now();
					auto timeEnd = std::chrono::system_clock::now();
					auto diff = timeEnd - timeStart;
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
					if (elapsed > 400)
					{
						ImVec2 cstart(lineStartScreenPos.x + mCharAdvance.x * (cx + cTextStart), lineStartScreenPos.y);
						ImVec2 cend(lineStartScreenPos.x + mCharAdvance.x * (cx + cTextStart) + (mOverwrite ? mCharAdvance.x : 1.0f), lineStartScreenPos.y + mCharAdvance.y);
						drawList->AddRectFilled(cstart, cend, mPalette[(int)PaletteIndex::Cursor]);
						if (elapsed > 800)
							timeStart = timeEnd;
					}
				}
			}

			appendIndex = 0;
			auto prevColor = line.empty() ? PaletteIndex::Default : (line[0].mMultiLineComment ? PaletteIndex::MultiLineComment : line[0].mColorIndex);

			for (auto& glyph : line)
			{
				auto color = glyph.mMultiLineComment ? PaletteIndex::MultiLineComment : glyph.mColorIndex;

				if (color != prevColor && !buffer.empty())
				{
					drawList->AddText(textScreenPos, mPalette[(uint8_t)prevColor], buffer.c_str());
					textScreenPos.x += mCharAdvance.x * buffer.length();
					buffer.clear();
					prevColor = color;
				}
				appendIndex = AppendBuffer(buffer, glyph.mChar, appendIndex);
				++columnNo;
			}

			if (!buffer.empty())
			{
				drawList->AddText(textScreenPos, mPalette[(uint8_t)prevColor], buffer.c_str());
				buffer.clear();
			}
			appendIndex = 0;
			lineStartScreenPos.y += mCharAdvance.y;
			textScreenPos.x = lineStartScreenPos.x + mCharAdvance.x * cTextStart;
			textScreenPos.y = lineStartScreenPos.y;
			++lineNo;
		}

		auto id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos()));
		if (!id.empty())
		{
			auto it = mLanguageDefinition.mIdentifiers.find(id);
			if (it != mLanguageDefinition.mIdentifiers.end())
			{
				ImGui::BeginTooltip();
				ImGui::TextUnformatted(it->second.mDeclaration.c_str());
				ImGui::EndTooltip();
			}
			else
			{
				auto pi = mLanguageDefinition.mPreprocIdentifiers.find(id);
				if (pi != mLanguageDefinition.mPreprocIdentifiers.end())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(pi->second.mDeclaration.c_str());
					ImGui::EndTooltip();
				}
			}
		}
	}


	ImGui::Dummy(ImVec2((longest + 2) * mCharAdvance.x, mLines.size() * mCharAdvance.y));

	if (mScrollToCursor)
	{
		EnsureCursorVisible();
		ImGui::SetWindowFocus();
		mScrollToCursor = false;
	}

	ImGui::PopAllowKeyboardFocus();
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	mWithinRender = false;
}

void TextEditor::SetText(const std::string & aText)
{
	mLines.clear();
	for (auto chr : aText)
	{
		if (mLines.empty())
			mLines.push_back(Line());
		if (chr == '\n')
			mLines.push_back(Line());
		else
		{
			mLines.back().push_back(Glyph(chr, PaletteIndex::Default));
		}
	}

	mUndoBuffer.clear();

	Colorize();
}

void TextEditor::EnterCharacter(Char aChar)
{
	assert(!mReadOnly);

	auto_complete_index = 0;

	UndoRecord u;

	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;
		DeleteSelection();
	}

	auto coord = GetActualCursorCoordinates();
	u.mAddedStart = coord;

	if (mLines.empty())
		mLines.push_back(Line());

	if (aChar == '\n')
	{
		InsertLine(coord.mLine + 1);
		auto& line = mLines[coord.mLine];
		auto& newLine = mLines[coord.mLine + 1];
		newLine.insert(newLine.begin(), line.begin() + coord.mColumn, line.end());
		line.erase(line.begin() + coord.mColumn, line.begin() + line.size());
		mState.mCursorPosition = Coordinates(coord.mLine + 1, 0);
	}
	else
	{
		auto& line = mLines[coord.mLine];
		if (mOverwrite && (int)line.size() < coord.mColumn)
			line[coord.mColumn] = Glyph(aChar, PaletteIndex::Default);
		else
			line.insert(line.begin() + coord.mColumn, Glyph(aChar, PaletteIndex::Default));
		mState.mCursorPosition = coord;
		++mState.mCursorPosition.mColumn;

		if (!is_method_auto_complete_open && !is_word_auto_complete_open && aChar != ' ' && aChar != '\t')
		{
			auto_complete_word.clear();
			auto_complete_word_methods.clear();
			auto_complete_word_list.clear();
			auto_complete_word_list_secondary.clear();
			int i = coord.mColumn;
			bool can_open = true;
			while (i > 0)
			{
				if (line[i - 1].mChar != ' ' && line[i - 1].mChar != '\t')
				{
					if (line[i - 1].mChar == ';')
					{
						break;
					}
					can_open = false;
					break;
				}
				i--;
			}
			if (can_open)
			{
				is_word_auto_complete_open = true;
			}
		}

		FillAutoWord(aChar);
	}

	u.mAdded = aChar;
	u.mAddedEnd = GetActualCursorCoordinates();
	u.mAfter = mState;

	AddUndo(u);

	Colorize(coord.mLine - 1, 3);
	EnsureCursorVisible();
}

void TextEditor::SetReadOnly(bool aValue)
{
	mReadOnly = aValue;
}

void TextEditor::SetCursorPosition(const Coordinates & aPosition)
{
	if (mState.mCursorPosition != aPosition)
	{
		mState.mCursorPosition = aPosition;
		EnsureCursorVisible();
	}
}

void TextEditor::SetSelectionStart(const Coordinates & aPosition)
{
	mState.mSelectionStart = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::SetSelectionEnd(const Coordinates & aPosition)
{
	mState.mSelectionEnd = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::SetSelection(const Coordinates & aStart, const Coordinates & aEnd, bool aWordMode)
{
	mState.mSelectionStart = SanitizeCoordinates(aStart);
	mState.mSelectionEnd = SanitizeCoordinates(aEnd);
	if (aStart > aEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);

	if (aWordMode)
	{
		mState.mSelectionStart = FindWordStart(mState.mSelectionStart);
		if (!IsOnWordBoundary(mState.mSelectionEnd))
			mState.mSelectionEnd = FindWordEnd(FindWordStart(mState.mSelectionEnd));
	}
}

void TextEditor::InsertText(const std::string & aValue)
{
	InsertText(aValue.c_str());
}

void TextEditor::InsertText(const char * aValue)
{
	if (aValue == nullptr)
		return;

	auto pos = GetActualCursorCoordinates();
	auto start = std::min(pos, mState.mSelectionStart);
	int totalLines = pos.mLine - start.mLine;

	totalLines += InsertTextAt(pos, aValue);

	SetSelection(pos, pos);
	SetCursorPosition(pos);
	Colorize(start.mLine - 1, totalLines + 2);
}

void TextEditor::DeleteSelection()
{
	assert(mState.mSelectionEnd >= mState.mSelectionStart);

	if (mState.mSelectionEnd == mState.mSelectionStart)
		return;

	DeleteRange(mState.mSelectionStart, mState.mSelectionEnd);

	SetSelection(mState.mSelectionStart, mState.mSelectionStart);
	SetCursorPosition(mState.mSelectionStart);
	Colorize(mState.mSelectionStart.mLine, 1);
}

void TextEditor::MoveUp(int aAmount, bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition.mLine = std::max(0, mState.mCursorPosition.mLine - aAmount);
	if (oldPos != mState.mCursorPosition)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);

		EnsureCursorVisible();
	}
}

void TextEditor::MoveDown(int aAmount, bool aSelect)
{
	assert(mState.mCursorPosition.mColumn >= 0);
	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + aAmount));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);

		EnsureCursorVisible();
	}
}

void TextEditor::MoveLeft(int aAmount, bool aSelect, bool aWordMode)
{
	if (mLines.empty())
		return;

	auto oldPos = mState.mCursorPosition;
	mState.mCursorPosition = GetActualCursorCoordinates();

	while (aAmount-- > 0)
	{
		if (mState.mCursorPosition.mColumn == 0)
		{
			if (mState.mCursorPosition.mLine > 0)
			{
				--mState.mCursorPosition.mLine;
				mState.mCursorPosition.mColumn = (int)mLines[mState.mCursorPosition.mLine].size();
			}
		}
		else
		{
			mState.mCursorPosition.mColumn = std::max(0, mState.mCursorPosition.mColumn - 1);
			if (aWordMode)
				mState.mCursorPosition = FindWordStart(mState.mCursorPosition);
		}
	}

	assert(mState.mCursorPosition.mColumn >= 0);
	if (aSelect)
	{
		if (oldPos == mInteractiveStart)
			mInteractiveStart = mState.mCursorPosition;
		else if (oldPos == mInteractiveEnd)
			mInteractiveEnd = mState.mCursorPosition;
		else
		{
			mInteractiveStart = mState.mCursorPosition;
			mInteractiveEnd = oldPos;
		}
	}
	else
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
	SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode);

	EnsureCursorVisible();
}

void TextEditor::MoveRight(int aAmount, bool aSelect, bool aWordMode)
{
	auto oldPos = mState.mCursorPosition;

	if (mLines.empty())
		return;

	while (aAmount-- > 0)
	{
		auto& line = mLines[mState.mCursorPosition.mLine];
		if (mState.mCursorPosition.mColumn >= (int)line.size())
		{
			mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + 1));
			mState.mCursorPosition.mColumn = 0;
		}
		else
		{
			mState.mCursorPosition.mColumn = std::max(0, std::min((int)line.size(), mState.mCursorPosition.mColumn + 1));
			if (aWordMode)
				mState.mCursorPosition = FindWordEnd(mState.mCursorPosition);
		}
	}

	if (aSelect)
	{
		if (oldPos == mInteractiveEnd)
			mInteractiveEnd = SanitizeCoordinates(mState.mCursorPosition);
		else if (oldPos == mInteractiveStart)
			mInteractiveStart = mState.mCursorPosition;
		else
		{
			mInteractiveStart = oldPos;
			mInteractiveEnd = mState.mCursorPosition;
		}
	}
	else
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
	SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode);

	EnsureCursorVisible();
}

void TextEditor::MoveTop(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(0, 0));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			mInteractiveEnd = oldPos;
			mInteractiveStart = mState.mCursorPosition;
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
}

void TextEditor::TextEditor::MoveBottom(bool aSelect)
{
	auto oldPos = GetCursorPosition();
	auto newPos = Coordinates((int)mLines.size() - 1, 0);
	SetCursorPosition(newPos);
	if (aSelect)
	{
		mInteractiveStart = oldPos;
		mInteractiveEnd = newPos;
	}
	else
		mInteractiveStart = mInteractiveEnd = newPos;
	SetSelection(mInteractiveStart, mInteractiveEnd);
}

void TextEditor::MoveHome(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, 0));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
}

void TextEditor::MoveEnd(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, (int)mLines[oldPos.mLine].size()));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
}

void TextEditor::Delete()
{
	assert(!mReadOnly);

	/*is_method_auto_complete_open = false;
	is_word_auto_complete_open = false;*/
	auto_complete_index = 0;

	if (mLines.empty())
		return;

	UndoRecord u;
	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();
	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);
		auto& line = mLines[pos.mLine];

		if (pos.mColumn == (int)line.size())
		{
			if (pos.mLine == (int)mLines.size() - 1)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			Advance(u.mRemovedEnd);

			auto& nextLine = mLines[pos.mLine + 1];
			line.insert(line.end(), nextLine.begin(), nextLine.end());
			RemoveLine(pos.mLine + 1);
		}
		else
		{
			u.mRemoved = line[pos.mColumn].mChar;
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			u.mRemovedEnd.mColumn++;

			line.erase(line.begin() + pos.mColumn);
		}

		Colorize(pos.mLine, 1);
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::BackSpace()
{
	assert(!mReadOnly);

	if (mLines.empty())
		return;


	UndoRecord u;
	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();
	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);

		if (mState.mCursorPosition.mColumn == 0)
		{
			if (mState.mCursorPosition.mLine == 0)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			Advance(u.mRemovedEnd);

			auto& line = mLines[mState.mCursorPosition.mLine];
			auto& prevLine = mLines[mState.mCursorPosition.mLine - 1];
			auto prevSize = (int)prevLine.size();
			prevLine.insert(prevLine.end(), line.begin(), line.end());
			RemoveLine(mState.mCursorPosition.mLine);
			--mState.mCursorPosition.mLine;
			mState.mCursorPosition.mColumn = prevSize;
		}
		else
		{
			auto& line = mLines[mState.mCursorPosition.mLine];

			u.mRemoved = line[pos.mColumn - 1].mChar;
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			--u.mRemovedStart.mColumn;

			--mState.mCursorPosition.mColumn;
			if (mState.mCursorPosition.mColumn < (int)line.size())
				line.erase(line.begin() + mState.mCursorPosition.mColumn);
		}
		EnsureCursorVisible();
		Colorize(mState.mCursorPosition.mLine, 1);

		if (!auto_complete_word.empty() && !is_method_auto_complete_open)
		{
			auto_complete_word.pop_back();
		}

		if (is_method_auto_complete_open)
		{
			if (!auto_complete_word_methods.empty())
			{
				auto_complete_word_methods.pop_back();
			}
		}
		
		FillAutoWord('\b');
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::SelectWordUnderCursor()
{
	auto c = GetCursorPosition();
	SetSelection(FindWordStart(c), FindWordEnd(c));
}

bool TextEditor::HasSelection() const
{
	return mState.mSelectionEnd > mState.mSelectionStart;
}

void TextEditor::Copy()
{
	if (HasSelection())
	{
		ImGui::SetClipboardText(GetSelectedText().c_str());
	}
	else
	{
		if (!mLines.empty())
		{
			std::string str;
			auto& line = mLines[GetActualCursorCoordinates().mLine];
			for (auto& g : line)
				str.push_back(g.mChar);
			ImGui::SetClipboardText(str.c_str());
		}
	}
}

void TextEditor::Cut()
{
	if (IsReadOnly())
	{
		Copy();
	}
	else
	{
		if (HasSelection())
		{
			UndoRecord u;
			u.mBefore = mState;
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;

			Copy();
			DeleteSelection();

			u.mAfter = mState;
			AddUndo(u);
		}
	}
}

void TextEditor::Paste()
{
	auto clipText = ImGui::GetClipboardText();
	if (clipText != nullptr && strlen(clipText) > 0)
	{
		UndoRecord u;
		u.mBefore = mState;

		if (HasSelection())
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}

		u.mAdded = clipText;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);
	}
}

bool TextEditor::CanUndo() const
{
	return mUndoIndex > 0;
}

bool TextEditor::CanRedo() const
{
	return mUndoIndex < (int)mUndoBuffer.size();
}

void TextEditor::Undo(int aSteps)
{
	while (CanUndo() && aSteps-- > 0)
		mUndoBuffer[--mUndoIndex].Undo(this);
}

void TextEditor::Redo(int aSteps)
{
	while (CanRedo() && aSteps-- > 0)
		mUndoBuffer[mUndoIndex++].Redo(this);
}

const TextEditor::Palette & TextEditor::GetDarkPalette()
{
	static Palette p = {
		0xffffffff,	// Default
		0xffd69c56,	// Keyword	
		0xff00ff00,	// Number
		0xff2092dd,	// String
		0xff70a0e0, // Char literal
		0xffffffff, // Punctuation
		0xff409090,	// Preprocessor
		0xffaaaaaa, // Identifier
		0xff9bc64d, // Known identifier
		0xffc040a0, // Preproc identifier
		0xff206020, // Comment (single line)
		0xff406020, // Comment (multi line)
		0xff101010, // Background
		0xffe0e0e0, // Cursor
		0x80a06020, // Selection
		0x800020ff, // ErrorMarker
		0x40f08000, // Breakpoint
		0xff707000, // Line number
		0x40000000, // Current line fill
		0x40808080, // Current line fill (inactive)
		0x40a0a0a0, // Current line edge
		0xffaabb00, // Classes

	};
	return p;
}

const TextEditor::Palette & TextEditor::GetLightPalette()
{
	static Palette p = {
		0xff000000,	// Default
		0xffff0c06,	// Keyword	
		0xff000000,	// Number
		0xff2092dd,	// String
		0xff304070, // Char literal
		0xff000000, // Punctuation
		0xff409090,	// Preprocessor
		0xff404040, // Identifier
		0xff606010, // Known identifier
		0xffc040a0, // Preproc identifier
		0xff205020, // Comment (single line)
		0xff405020, // Comment (multi line)
		0xffffffff, // Background
		0xff000000, // Cursor
		0x80600000, // Selection
		0xa00010ff, // ErrorMarker
		0x80f08000, // Breakpoint
		0xff505000, // Line number
		0x40000000, // Current line fill
		0x40808080, // Current line fill (inactive)
		0x40000000, // Current line edge
		0xffaabb00, // Classes
	};
	return p;
}

std::string TextEditor::GetText() const
{
	return GetText(Coordinates(), Coordinates((int)mLines.size(), 0));
}

std::string TextEditor::GetSelectedText() const
{
	return GetText(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::ProcessInputs()
{
}

void TextEditor::Colorize(int aFromLine, int aLines)
{
	int toLine = aLines == -1 ? (int)mLines.size() : std::min((int)mLines.size(), aFromLine + aLines);
	mColorRangeMin = std::min(mColorRangeMin, aFromLine);
	mColorRangeMax = std::max(mColorRangeMax, toLine);
	mColorRangeMin = std::max(0, mColorRangeMin);
	mColorRangeMax = std::max(mColorRangeMin, mColorRangeMax);
	mCheckMultilineComments = true;
}

void TextEditor::ColorizeRange(int aFromLine, int aToLine)
{
	if (mLines.empty() || aFromLine >= aToLine)
		return;

	std::string buffer;
	int endLine = std::max(0, std::min((int)mLines.size(), aToLine));
	for (int i = aFromLine; i < endLine; ++i)
	{
		bool preproc = false;
		auto& line = mLines[i];
		buffer.clear();
		for (auto g : mLines[i])
		{
			buffer.push_back(g.mChar);
			g.mColorIndex = PaletteIndex::Default;
		}

		std::match_results<std::string::const_iterator> results;
		auto last = buffer.cend();
		for (auto first = buffer.cbegin(); first != last; ++first)
		{
			for (auto& p : mRegexList)
			{
				if (std::regex_search<std::string::const_iterator>(first, last, results, p.first, std::regex_constants::match_continuous))
				{
					auto v = *results.begin();
					static TextEditor::LanguageDefinition def = GetLanguageDefinition();
					if (def.mClasses.find(v.str()) != def.mClasses.end())
					{
						std::string tmp = v.first._Ptr;
						if (tmp.find(' ') != std::string::npos && tmp.find('.') == std::string::npos && tmp.find(';') != std::string::npos)
						{
							std::string sentence = v.second._Ptr;
							std::string variable_name;
							bool variable_started = false;
							for (std::string::iterator it = sentence.begin(); it != sentence.end(); it++)
							{
								if (*it != ' ' && *it != ';' && *it != '.')
								{
									variable_started = true;
								}
								else
								{
									if (variable_started)
									{
										break;
									}
								}
								if (variable_started)
								{
									variable_name.push_back(*it);
								}
							}
							if (variables.find(variable_name) == variables.end())
							{
								variables[variable_name] = v.str();
								custom_variables[variable_name] = v.str();
							}
						}
					}
					auto start = v.first - buffer.begin();
					auto end = v.second - buffer.begin();
					auto id = buffer.substr(start, end - start);
					auto color = p.second;
					if (color == PaletteIndex::Identifier)
					{
						if (!mLanguageDefinition.mCaseSensitive)
							std::transform(id.begin(), id.end(), id.begin(), ::toupper);

						if (!preproc)
						{
							if (mLanguageDefinition.mKeywords.find(id) != mLanguageDefinition.mKeywords.end())
								color = PaletteIndex::Keyword;
							else if (mLanguageDefinition.mIdentifiers.find(id) != mLanguageDefinition.mIdentifiers.end())
								color = PaletteIndex::KnownIdentifier;
							else if (mLanguageDefinition.mPreprocIdentifiers.find(id) != mLanguageDefinition.mPreprocIdentifiers.end())
								color = PaletteIndex::PreprocIdentifier;
							else if (mLanguageDefinition.mClasses.find(id) != mLanguageDefinition.mClasses.end() && buffer.find("using") == std::string::npos)
								color = PaletteIndex::Classes;
						}
						else
						{
							if (mLanguageDefinition.mPreprocIdentifiers.find(id) != mLanguageDefinition.mPreprocIdentifiers.end())
								color = PaletteIndex::PreprocIdentifier;
							else
								color = PaletteIndex::Identifier;
						}
					}
					else if (color == PaletteIndex::Preprocessor)
					{
						preproc = true;
					}
					for (int j = (int)start; j < (int)end; ++j)
						line[j].mColorIndex = color;
					first += end - start - 1;
					break;
				}
			}
		}
	}
}

void TextEditor::ColorizeInternal()
{
	if (mLines.empty())
		return;

	if (mCheckMultilineComments)
	{
		auto end = Coordinates((int)mLines.size(), 0);
		auto commentStart = end;
		auto withinString = false;
		for (auto i = Coordinates(0, 0); i < end; Advance(i))
		{
			auto& line = mLines[i.mLine];
			if (!line.empty())
			{
				auto g = line[i.mColumn];
				auto c = g.mChar;

				bool inComment = commentStart <= i;

				if (withinString)
				{
					line[i.mColumn].mMultiLineComment = inComment;

					if (c == '\"')
					{
						if (i.mColumn + 1 < (int)line.size() && line[i.mColumn + 1].mChar == '\"')
						{
							Advance(i);
							if (i.mColumn < (int)line.size())
								line[i.mColumn].mMultiLineComment = inComment;
						}
						else
							withinString = false;
					}
					else if (c == '\\')
					{
						Advance(i);
						if (i.mColumn < (int)line.size())
							line[i.mColumn].mMultiLineComment = inComment;
					}
				}
				else
				{
					if (c == '\"')
					{
						withinString = true;
						line[i.mColumn].mMultiLineComment = inComment;
					}
					else
					{
						auto pred = [](const char& a, const Glyph& b) { return a == b.mChar; };
						auto from = line.begin() + i.mColumn;
						auto& startStr = mLanguageDefinition.mCommentStart;
						if (i.mColumn + startStr.size() <= line.size() &&
							std::equal(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
							commentStart = i;

						inComment = commentStart <= i;

						line[i.mColumn].mMultiLineComment = inComment;

						auto& endStr = mLanguageDefinition.mCommentEnd;
						if (i.mColumn + 1 >= (int)endStr.size() &&
							std::equal(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
							commentStart = end;
					}
				}
			}
		}
		mCheckMultilineComments = false;
		return;
	}

	if (mColorRangeMin < mColorRangeMax)
	{
		int to = std::min(mColorRangeMin + 10, mColorRangeMax);
		ColorizeRange(mColorRangeMin, to);
		mColorRangeMin = to;

		if (mColorRangeMax == mColorRangeMin)
		{
			mColorRangeMin = std::numeric_limits<int>::max();
			mColorRangeMax = 0;
		}
		return;
	}
}

int TextEditor::TextDistanceToLineStart(const Coordinates& aFrom) const
{
	auto& line = mLines[aFrom.mLine];
	auto len = 0;
	for (size_t it = 0u; it < line.size() && it < (unsigned)aFrom.mColumn; ++it)
		len = line[it].mChar == '\t' ? (len / mTabSize) * mTabSize + mTabSize : len + 1;
	return len;
}

void TextEditor::EnsureCursorVisible()
{
	if (!mWithinRender)
	{
		mScrollToCursor = true;
		return;
	}

	float scrollX = ImGui::GetScrollX();
	float scrollY = ImGui::GetScrollY();

	auto height = ImGui::GetWindowHeight();
	auto width = ImGui::GetWindowWidth();

	auto top = 1 + (int)ceil(scrollY / mCharAdvance.y);
	auto bottom = (int)ceil((scrollY + height) / mCharAdvance.y);

	auto left = (int)ceil(scrollX / mCharAdvance.x);
	auto right = (int)ceil((scrollX + width) / mCharAdvance.x);

	auto pos = GetActualCursorCoordinates();
	auto len = TextDistanceToLineStart(pos);

	if (pos.mLine < top)
		ImGui::SetScrollY(std::max(0.0f, (pos.mLine - 1) * mCharAdvance.y));
	if (pos.mLine > bottom - 4)
		ImGui::SetScrollY(std::max(0.0f, (pos.mLine + 4) * mCharAdvance.y - height));
	if (len + cTextStart < left + 4)
		ImGui::SetScrollX(std::max(0.0f, (len + cTextStart - 4) * mCharAdvance.x));
	if (len + cTextStart > right - 4)
		ImGui::SetScrollX(std::max(0.0f, (len + cTextStart + 4) * mCharAdvance.x - width));
}

int TextEditor::GetPageSize() const
{
	auto height = ImGui::GetWindowHeight() - 20.0f;
	return (int)floor(height / mCharAdvance.y);
}

TextEditor::UndoRecord::UndoRecord(
	const std::string& aAdded,
	const TextEditor::Coordinates aAddedStart,
	const TextEditor::Coordinates aAddedEnd,
	const std::string& aRemoved,
	const TextEditor::Coordinates aRemovedStart,
	const TextEditor::Coordinates aRemovedEnd,
	TextEditor::EditorState& aBefore,
	TextEditor::EditorState& aAfter)
	: mAdded(aAdded)
	, mAddedStart(aAddedStart)
	, mAddedEnd(aAddedEnd)
	, mRemoved(aRemoved)
	, mRemovedStart(aRemovedStart)
	, mRemovedEnd(aRemovedEnd)
	, mBefore(aBefore)
	, mAfter(aAfter)
{
	assert(mAddedStart <= mAddedEnd);
	assert(mRemovedStart <= mRemovedEnd);
}

void TextEditor::UndoRecord::Undo(TextEditor * aEditor)
{
	if (!mAdded.empty())
	{
		aEditor->DeleteRange(mAddedStart, mAddedEnd);
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 2);
	}

	if (!mRemoved.empty())
	{
		auto start = mRemovedStart;
		aEditor->InsertTextAt(start, mRemoved.c_str());
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 2);
	}

	aEditor->mState = mBefore;
	aEditor->EnsureCursorVisible();

}

void TextEditor::UndoRecord::Redo(TextEditor * aEditor)
{
	if (!mRemoved.empty())
	{
		aEditor->DeleteRange(mRemovedStart, mRemovedEnd);
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 1);
	}

	if (!mAdded.empty())
	{
		auto start = mAddedStart;
		aEditor->InsertTextAt(start, mAdded.c_str());
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 1);
	}

	aEditor->mState = mAfter;
	aEditor->EnsureCursorVisible();
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::CPlusPlus()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const cppKeywords[] = {
			"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char16_t", "char32_t", "class",
			"compl", "concept", "const", "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
			"for", "friend", "goto", "if", "import", "inline", "int", "long", "module", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
			"register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this", "thread_local",
			"throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
		};
		for (auto& k : cppKeywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf", "sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper",
			"std", "string", "vector", "map", "unordered_map", "set", "unordered_set", "min", "max"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "C++";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::HLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment",
			"CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword", "else",
			"export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
			"linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
			"pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState", "RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
			"RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state",
			"static", "string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS",
			"Texture2DMSArray", "Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment",
			"VertexShader", "void", "volatile", "while",
			"bool1","bool2","bool3","bool4","double1","double2","double3","double4", "float1", "float2", "float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout",
			"uint1", "uint2", "uint3", "uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4",
			"float1x1","float2x1","float3x1","float4x1","float1x2","float2x2","float3x2","float4x2",
			"float1x3","float2x3","float3x3","float4x3","float1x4","float2x4","float3x4","float4x4",
			"half1x1","half2x1","half3x1","half4x1","half1x2","half2x2","half3x2","half4x2",
			"half1x3","half2x3","half3x3","half4x3","half1x4","half2x4","half3x4","half4x4",
			//new
			"using", "public", "private"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint", "asint", "asuint",
			"asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross", "D3DCOLORtoUBYTE4", "ddx",
			"ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync",
			"distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "exp", "exp2",
			"f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma", "fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount",
			"GetRenderTargetSamplePosition", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange",
			"InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan",
			"ldexp", "length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf",
			"Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg",
			"ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin",
			"radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step",
			"tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod", "tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj",
			"tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod", "tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "HLSL";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::GLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"in", "out", "float", "double", "int", "void", "bool", "true", "false",
			"invariant",
			"discard", "return",
			"mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4",
			"mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4",
			"mat3x2", "mat3x3", "mat3x4", "dmat3x2", "dmat3x3", "dmat3x4",
			"mat4x2", "mat4x3", "mat4x4", "dmat4x2", "dmat4x3", "dmat4x4",
			"vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "bvec2", "bvec3", "bvec4", "dvec2", "dvec3", "dvec4",
			"uint", "uvec2", "uvec3", "uvec4",
			"lowp", "mediump", "highp", "precision",
			"sampler1D", "sampler2D", "sampler3D", "samplerCube",
			"sampler1DShadow", "sampler2DShadow", "samplerCubeShadow",
			"sampler1DArray", "sampler2DArray",
			"sampler1DArrayShadow", "sampler2DArrayShadow",
			"isampler1D", "isampler2D", "isampler3D", "isamplerCube",
			"isampler1DArray", "isampler2DArray",
			"usampler1D", "usampler2D", "usampler3D", "usamplerCube",
			"usampler1DArray", "usampler2DArray",
			"sampler2DRect", "sampler2DRectShadow", "isampler2DRect", "usampler2DRect",
			"samplerBuffer", "isamplerBuffer", "usamplerBuffer",
			"sampler2DMS", "isampler2DMS", "usampler2DMS",
			"sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray",
			"samplerCubeArray", "samplerCubeArrayShadow", "isamplerCubeArray", "usamplerCubeArray",
			"struct"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"gl_MaxVertexAttribs", "gl_MaxVertexOutputComponents", "gl_MaxVertexUniformComponents", "gl_MaxVertexTextureImageUnits", "gl_MaxGeometryInputComponents", 
			"gl_MaxGeometryOutputComponents", "gl_MaxGeometryUniformComponents", "gl_MaxGeometryTextureImageUnits", "gl_MaxGeometryOutputVertices", "gl_MaxGeometryTotalOutputComponents", 
			"gl_MaxGeometryVaryingComponents", "gl_MaxFragmentInputComponents", "gl_MaxDrawBuffers", "gl_MaxFragmentUniformComponents", "gl_MaxTextureImageUnits", "gl_MaxClipDistances", 
			"gl_MaxCombinedTextureImageUnits", "gl_MaxTessControlInputComponents", "gl_MaxTessControlOutputComponents", "gl_MaxTessControlUniformComponents", "gl_MaxTessControlTextureImageUnits", 
			"gl_MaxTessControlTotalOutputComponents", "gl_MaxTessEvaluationInputComponents", "gl_MaxTessEvaluationOutputComponents", "gl_MaxTessEvaluationUniformComponents", 
			"gl_MaxTessEvaluationTextureImageUnits", "gl_MaxTessPatchComponents", "gl_MaxPatchVertices", "gl_MaxTessGenLevel", "gl_MaxViewports", "gl_MaxVertexUniformVectors", "gl_MaxFragmentUniformVectors",
			"gl_MaxVaryingVectors", "gl_MaxVertexImageUniforms", "gl_MaxVertexAtomicCounters", "gl_MaxVertexAtomicCounterBuffers", "gl_MaxTessControlImageUniforms", 
			"gl_MaxTessControlAtomicCounters", "gl_MaxTessControlAtomicCounterBuffers", "gl_MaxTessEvaluationImageUniforms", "gl_MaxTessEvaluationAtomicCounters",
			"gl_MaxTessEvaluationAtomicCounterBuffers", "gl_MaxGeometryImageUniforms", "gl_MaxGeometryAtomicCounters", "gl_MaxGeometryAtomicCounterBuffers", "gl_MaxFragmentImageUniforms",
			"gl_MaxFragmentAtomicCounters", "gl_MaxFragmentAtomicCounterBuffers", "gl_MaxCombinedImageUniforms", "gl_MaxCombinedAtomicCounters", "gl_MaxCombinedAtomicCounterBuffers",
			"gl_MaxImageUnits", "gl_MaxCombinedImageUnitsAndFragmentOutputs", "gl_MaxImageSamples", "gl_MaxAtomicCounterBindings", "gl_MaxAtomicCounterBufferSize",
			"gl_MinProgramTexelOffset", "gl_MaxProgramTexelOffset", "gl_VertexID", "gl_InstanceID", "gl_Position", "gl_PointSize", "gl_ClipDistance", "gl_PatchVerticesIn",
			"gl_PrimitiveID", "gl_InvocationID", "gl_TessLevelOuter", "gl_TessLevelInner", "gl_TessCoord", "gl_PatchVerticesIn", "gl_PrimitiveID", "gl_PrimitiveIDIn",
			"gl_InvocationID", "gl_Layer", "gl_ViewportIndex", "gl_FragCoord", "gl_FrontFacing", "gl_PointCoord", "gl_SampleID", "gl_SamplePosition", "gl_SampleMaskIn",
			"gl_FragDepth", "gl_NumWorkGroups", "gl_WorkGroupID", "gl_LocalInvocationID", "gl_GlobalInvocationID", "gl_LocalInvocationIndex"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "GLSL";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::C()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
			"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
			"_Noreturn", "_Static_assert", "_Thread_local"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "C";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::SQL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"ADD", "EXCEPT", "PERCENT", "ALL", "EXEC", "PLAN", "ALTER", "EXECUTE", "PRECISION", "AND", "EXISTS", "PRIMARY", "ANY", "EXIT", "PRINT", "AS", "FETCH", "PROC", "ASC", "FILE", "PROCEDURE",
			"AUTHORIZATION", "FILLFACTOR", "PUBLIC", "BACKUP", "FOR", "RAISERROR", "BEGIN", "FOREIGN", "READ", "BETWEEN", "FREETEXT", "READTEXT", "BREAK", "FREETEXTTABLE", "RECONFIGURE",
			"BROWSE", "FROM", "REFERENCES", "BULK", "FULL", "REPLICATION", "BY", "FUNCTION", "RESTORE", "CASCADE", "GOTO", "RESTRICT", "CASE", "GRANT", "RETURN", "CHECK", "GROUP", "REVOKE",
			"CHECKPOINT", "HAVING", "RIGHT", "CLOSE", "HOLDLOCK", "ROLLBACK", "CLUSTERED", "IDENTITY", "ROWCOUNT", "COALESCE", "IDENTITY_INSERT", "ROWGUIDCOL", "COLLATE", "IDENTITYCOL", "RULE",
			"COLUMN", "IF", "SAVE", "COMMIT", "IN", "SCHEMA", "COMPUTE", "INDEX", "SELECT", "CONSTRAINT", "INNER", "SESSION_USER", "CONTAINS", "INSERT", "SET", "CONTAINSTABLE", "INTERSECT", "SETUSER",
			"CONTINUE", "INTO", "SHUTDOWN", "CONVERT", "IS", "SOME", "CREATE", "JOIN", "STATISTICS", "CROSS", "KEY", "SYSTEM_USER", "CURRENT", "KILL", "TABLE", "CURRENT_DATE", "LEFT", "TEXTSIZE",
			"CURRENT_TIME", "LIKE", "THEN", "CURRENT_TIMESTAMP", "LINENO", "TO", "CURRENT_USER", "LOAD", "TOP", "CURSOR", "NATIONAL", "TRAN", "DATABASE", "NOCHECK", "TRANSACTION",
			"DBCC", "NONCLUSTERED", "TRIGGER", "DEALLOCATE", "NOT", "TRUNCATE", "DECLARE", "NULL", "TSEQUAL", "DEFAULT", "NULLIF", "UNION", "DELETE", "OF", "UNIQUE", "DENY", "OFF", "UPDATE",
			"DESC", "OFFSETS", "UPDATETEXT", "DISK", "ON", "USE", "DISTINCT", "OPEN", "USER", "DISTRIBUTED", "OPENDATASOURCE", "VALUES", "DOUBLE", "OPENQUERY", "VARYING","DROP", "OPENROWSET", "VIEW",
			"DUMMY", "OPENXML", "WAITFOR", "DUMP", "OPTION", "WHEN", "ELSE", "OR", "WHERE", "END", "ORDER", "WHILE", "ERRLVL", "OUTER", "WITH", "ESCAPE", "OVER", "WRITETEXT"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"ABS",  "ACOS",  "ADD_MONTHS",  "ASCII",  "ASCIISTR",  "ASIN",  "ATAN",  "ATAN2",  "AVG",  "BFILENAME",  "BIN_TO_NUM",  "BITAND",  "CARDINALITY",  "CASE",  "CAST",  "CEIL",
			"CHARTOROWID",  "CHR",  "COALESCE",  "COMPOSE",  "CONCAT",  "CONVERT",  "CORR",  "COS",  "COSH",  "COUNT",  "COVAR_POP",  "COVAR_SAMP",  "CUME_DIST",  "CURRENT_DATE",
			"CURRENT_TIMESTAMP",  "DBTIMEZONE",  "DECODE",  "DECOMPOSE",  "DENSE_RANK",  "DUMP",  "EMPTY_BLOB",  "EMPTY_CLOB",  "EXP",  "EXTRACT",  "FIRST_VALUE",  "FLOOR",  "FROM_TZ",  "GREATEST",
			"GROUP_ID",  "HEXTORAW",  "INITCAP",  "INSTR",  "INSTR2",  "INSTR4",  "INSTRB",  "INSTRC",  "LAG",  "LAST_DAY",  "LAST_VALUE",  "LEAD",  "LEAST",  "LENGTH",  "LENGTH2",  "LENGTH4",
			"LENGTHB",  "LENGTHC",  "LISTAGG",  "LN",  "LNNVL",  "LOCALTIMESTAMP",  "LOG",  "LOWER",  "LPAD",  "LTRIM",  "MAX",  "MEDIAN",  "MIN",  "MOD",  "MONTHS_BETWEEN",  "NANVL",  "NCHR",
			"NEW_TIME",  "NEXT_DAY",  "NTH_VALUE",  "NULLIF",  "NUMTODSINTERVAL",  "NUMTOYMINTERVAL",  "NVL",  "NVL2",  "POWER",  "RANK",  "RAWTOHEX",  "REGEXP_COUNT",  "REGEXP_INSTR",
			"REGEXP_REPLACE",  "REGEXP_SUBSTR",  "REMAINDER",  "REPLACE",  "ROUND",  "ROWNUM",  "RPAD",  "RTRIM",  "SESSIONTIMEZONE",  "SIGN",  "SIN",  "SINH",
			"SOUNDEX",  "SQRT",  "STDDEV",  "SUBSTR",  "SUM",  "SYS_CONTEXT",  "SYSDATE",  "SYSTIMESTAMP",  "TAN",  "TANH",  "TO_CHAR",  "TO_CLOB",  "TO_DATE",  "TO_DSINTERVAL",  "TO_LOB",
			"TO_MULTI_BYTE",  "TO_NCLOB",  "TO_NUMBER",  "TO_SINGLE_BYTE",  "TO_TIMESTAMP",  "TO_TIMESTAMP_TZ",  "TO_YMINTERVAL",  "TRANSLATE",  "TRIM",  "TRUNC", "TZ_OFFSET",  "UID",  "UPPER",
			"USER",  "USERENV",  "VAR_POP",  "VAR_SAMP",  "VARIANCE",  "VSIZE "
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\-\\-.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = false;

		langDef.mName = "SQL";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::AngelScript()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "abstract", "auto", "bool", "break", "case", "cast", "class", "const", "continue", "default", "do", "double", "else", "enum", "false", "final", "float", "for",
			"from", "funcdef", "function", "get", "if", "import", "in", "inout", "int", "interface", "int8", "int16", "int32", "int64", "is", "mixin", "namespace", "not",
			"null", "or", "out", "override", "private", "protected", "return", "set", "shared", "super", "switch", "this ", "true", "typedef", "uint", "uint8", "uint16", "uint32",
			"uint64", "void", "while", "xor"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"cos", "sin", "tab", "acos", "asin", "atan", "atan2", "cosh", "sinh", "tanh", "log", "log10", "pow", "sqrt", "abs", "ceil", "floor", "fraction", "closeTo", "fpFromIEEE", "fpToIEEE",
			"complex", "opEquals", "opAddAssign", "opSubAssign", "opMulAssign", "opDivAssign", "opAdd", "opSub", "opMul", "opDiv"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "AngelScript";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::Lua()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
			"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace",
			"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable",
			"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen",
			"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
			"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
			"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
			"date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
			"reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
			"coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\-\\-.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "\\-\\-\\[\\[";
		langDef.mCommentEnd = "\\]\\]";

		langDef.mCaseSensitive = true;

		langDef.mName = "Lua";

		inited = true;
	}
	return langDef;
}

TextEditor::LanguageDefinition TextEditor::LanguageDefinition::CSharp()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const csharpKeywords[] = {
			"abstract","as","base","bool","break","byte","case","catch","char","checked","class","const","continue","decimal","default","default","delegate","do","double","else","enum","event","explicit","extern","false","finally",
			"fixed","float","for","foreach","goto","if","implicit","in","int","interace","internal","is","lock","long","namescape","new","null","object","operator","out","override","params","private","protected","public",
			"readonly","ref","return","sbyte","sealed","short","sizeof","stackalloc","static","string","struct","switch","this","throw","true","try","typeof","uint","ulong","unchecked","unsafe","ushort","using","using static","virtual","void","volatile","while"
		};

		static const char* const csharpClasses[] = {
			"Math","TheComponent","TheConsole","TheFactory","TheGameObject","TheInput","TheQuaternion","TheTransform","TheVector3","Time","TheRectTransform","TheProgressBar"
		};
		for (auto& k : csharpClasses)
			langDef.mClasses.insert(k);

		for (auto& k : csharpKeywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"@Override"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.class_static_auto_complete["Math"] = "Abs()_Acos()_Asin()_Atan()_Atan2()_Ceiling()_Clamp()_Cos()_DegToRad_Epsilon_Exp()_Floor()_Infinity_IsPowerOfTwo()_Lerp()_Log()_Max()_Min()_NegativeInfinity_PI_Pow()_RadToDeg_Round()_Sin()_Sqrt()_Tan()";
		langDef.class_static_auto_complete["TheGameObject"] = "Destroy()_Duplicate()_Self";
		langDef.class_non_static_auto_complete["TheGameObject"] = "AddComponent<TheFactory>()_GetChild()_GetChildCount()_GetComponent<TheTransform>()_GetComponent<TheFactory>()_IsActive()_IsStatic()_layer_name_SetActive()_SetParent()_SetStatic()_tag_GetComponent<TheRectTransform>()_GetComponent<TheProgressBar>()";
		langDef.class_static_auto_complete["TheConsole"] = "Error()_Log()_Warning()";
		langDef.class_non_static_auto_complete["TheFactory"] = "SetSpawnPosition()_SetSpawnRotation()_SetSpawnScale()_Spawn()_StartFactory()";
		langDef.class_static_auto_complete["TheInput"] = "GetMousePosition()_GetMouseXMotion()_GetMouseYMotion()_IsKeyDown()_IsKeyRepeat()_IsKeyUp()_IsMouseButtonDown()_IsMouseButtonRepeat()_IsMouseButtonUp()";
		langDef.class_static_auto_complete["TheQuaternion"] = "AngleBetween()_DotProduct()_FromAngleAxis()_FromEulerAngles()_Identity_Lerp()_LookRotation()_Magnitude()_Normalize()_RotateTowards()_Slerp()_ToAngleAxis()";
		langDef.class_non_static_auto_complete["TheQuaternion"] = "Conjugate()_Inverse()_Length_LengthSquared_Normalized_Set()_ToAngleAxis()_ToEulerAngles()_ToString()_w_x_y_z";
		langDef.class_non_static_auto_complete["TheTransform"] = "ForwardDirection_GlobalPosition_GlobalRotation_GlobalScale_LocalPosition_LocalRotation_LocalScale_LookAt()_ToString()";
		langDef.class_static_auto_complete["TheVector3"] = "AngleBetween()_BackWard_CrossProduct()_Distance()_DotProduct()_Down_Forward_Left_Lerp()_Magnitude()_MoveTowards()_NLerp()_Normalize()_Project()_Reflect()_Right_Scale()_SLerp()_Up_Zero";
		langDef.class_non_static_auto_complete["TheVector3"] = "Length_LengthSquared_Normalized_Scale()_Set()_ToQuaternion()_ToString()_x_y_z";
		langDef.class_static_auto_complete["Time"] = "TimeScale_DeltaTime";
		langDef.class_non_static_auto_complete["TheProgressBar"] = "PercentageProgress";

		langDef.functions_info["Abs()"] = "Returns the absolute value of value. \n1. float Abs(float value) \n2. int Abs(int value)";
		langDef.functions_info["Acos()"] = "Returns the arc-cosine of value. \nfloat Acos(float value)";
		langDef.functions_info["Asin()"] = "Returns the arc-sine of value. \nfloat Asin(float value)";
		langDef.functions_info["Atan()"] = "Returns the arc-tangent of value. \nfloat Atan(float value)";
		langDef.functions_info["Atan2()"] = "Returns the angle in radians whose Tan is a/b. \nfloat Atan2(float a, float b)";
		langDef.functions_info["Ceiling()"] = "Returns the smallest integer greater to or equal to value. \nfloat Ceiling(float value)";
		langDef.functions_info["Clamp()"] = "Clamps a value between a minimum float and maximum float value. \nfloat Clamp(float value, float max_value, float min_value)";
		langDef.functions_info["Cos()"] = "Returns the cosine of angle value in radians. \nfloat Cos(float value)";
		langDef.functions_info["DegToRad"] = "Degrees to Radians. \n float DegToRad = 0.0174532925199432957f";
		langDef.functions_info["Epsilon"] = "A tiny floating point value. \nfloat Epsilon = 1.401298E-45f";
		langDef.functions_info["Exp()"] = "Returns e raised to the specified power. \nfloat Exp(float power)";
		langDef.functions_info["Floor()"] = "Returns the largest integer smaller to or equal to value. \nfloat Floor(float value)";
		langDef.functions_info["Infinity"] = "A representation of positive infinity. \nfloat Infinity = float.PositiveInfinity;";
		langDef.functions_info["IsPowerOfTwo()"] = "Returns true if the value is power of two. \nbool IsPowerOfTwo(int value)";
		langDef.functions_info["Lerp()_Math"] = "Linearly interpolates between start and end by value. \nfloat Lerp(float start, float end, float value)";
		langDef.functions_info["Log()"] = "Returns the logarithm of a specified number in a specified base. \nfloat Log(float value, float new_base)";
		langDef.functions_info["Max()"] = "Returns largest of two values. \nfloat Max(float a, float b)";
		langDef.functions_info["Min()"] = "Returns smallest of two values. \nfloat Min(float a, float b)";
		langDef.functions_info["Min()"] = "Returns smallest of two values. \nfloat Min(float a, float b)";
		langDef.functions_info["NegativeInfinity"] = "A representation of negative infinity. NegativeInfinity = float.NegativeInfinity";
		langDef.functions_info["PI"] = "The PI value. \nPI = 3.14159265358979323846.";
		langDef.functions_info["Pow()"] = "Returns value raised to power. \nfloat Pow(float value, float power)";
		langDef.functions_info["RadToDeg"] = "Radians to Degrees. \nRadToDeg = 57.295779513082320876f";
		langDef.functions_info["Round()"] = "Returns value rounded to the nearest integer. \nfloat Round(float value)";
		langDef.functions_info["Sin()"] = "Returns the sine of angle value in radians. \nfloat Sin(float value)";
		langDef.functions_info["Sqrt()"] = "Returns square root of value. \nfloat Sqrt(float value)";
		langDef.functions_info["Tan()"] = "Returns the tangent of angle value in radians. \nfloat Tan(float value)";
		langDef.functions_info["Destroy()"] = "Destroys the target GameObject. \nVoid Destroy(TheGameObject target)";
		langDef.functions_info["Duplicate()"] = "Duplicates the original GameObject. \nTheGameObject Duplicate(TheGameObject original)";
		langDef.functions_info["Self"] = "The GameObject this script is attached to. \nTheGameObject Self";
		langDef.functions_info["AddComponent<TheFactory>()"] = "Adds a Factory component to the attached GameObject. \nTheFactory AddComponent<TheFactory>()";
		langDef.functions_info["GetChild()"] = "Returns the child by index. \n1. TheGameObject GetChild(int index) \n2. TheGameObject GetChild(string child_name)";
		langDef.functions_info["GetChildCount()"] = "Returns the numbers of childs. \nint GetChildCount()";
		langDef.functions_info["GetComponent<TheTransform>()"] = "Returns the transform attached to this GameObject. \nTheTransform GetComponent<TheTransform>()";
		langDef.functions_info["GetComponent<TheFactory>()"] = "Returns the Factory attached to this GameObject or null if component doesn't exist. \nTheFactory GetComponent<TheFactory>()";
		langDef.functions_info["IsActive()"] = "Returns GameObject active state. \nbool IsActive()";
		langDef.functions_info["IsStatic()"] = "Returns GameObject static state. \nbool IsStatic()";
		langDef.functions_info["layer"] = "GameObject layer. \nstring layer";
		langDef.functions_info["name"] = "GameObject name. \n string name";
		langDef.functions_info["SetActive()"] = "Activates/Deactivates the GameObject. \nvoid SetActive(bool value)";
		langDef.functions_info["SetParent()"] = "Set the parent of the GameObject. \nvoid SetParent(TheGameObject new_parent)";
		langDef.functions_info["SetStatic()"] = "Set the GameObject static state. \nvoid SetStatic(bool value)";
		langDef.functions_info["tag"] = "GameObject tag. \nstring tag";
		langDef.functions_info["Error()"] = "Prints message in engine console as error. \nvoid Error(object message)";
		langDef.functions_info["Warning()"] = "Prints message in engine console as warning. \nvoid Warning(object message)";
		langDef.functions_info["Log()"] = "Prints message in engine console as log. \nvoid Log(object message)";
		langDef.functions_info["SetSpawnPosition()"] = "Set the position for the following GameObjects to spawn. \nvoid SetSpawnPosition(TheVector3 position)";
		langDef.functions_info["SetSpawnRotation()"] = "Set the rotation for the following GameObjects to spawn. \nvoid SetSpawnRotation(TheVector3 rotation)";
		langDef.functions_info["SetSpawnScale()"] = "Set the scale for the following GameObjects to spawn. \nvoid SetSpawnScale(TheVector3 scale)";
		langDef.functions_info["Spawn()"] = "Spawn the next GameObject. \nTheGameObject Spawn()";
		langDef.functions_info["StartFactory()"] = "Starts the factory component. All GameObjects with the component values will be created. \nvoid StartFactory()";
		langDef.functions_info["GetMousePosition()"] = "Returns the mouse position. \nTheVector3 GetMousePosition()";
		langDef.functions_info["GetMouseXMotion()"] = "Returns the mouse x movement. \nint GetMouseXMotion()";
		langDef.functions_info["GetMouseYMotion()"] = "Returns the mouse y movement. \nint GetMouseYMotion()";
		langDef.functions_info["IsKeyDown()"] = "Returns if the key is pressed. \nbool IsKeyDown(string key_name)";
		langDef.functions_info["IsKeyRepeat()"] = "Returns if the key is hold down. \nbool IsKeyRepeat(string key_name)";
		langDef.functions_info["IsKeyUp()"] = "Returns if the key is released. \nbool IsKeyUp(string key_name)";
		langDef.functions_info["IsMouseButtonDown()"] = "Returns if the mouse button is pressed. \nbool IsMouseButtonDown(int mouse_button)";
		langDef.functions_info["IsMouseButtonRepeat()"] = "Returns if the mouse button is hold down. \nbool IsMouseButtonRepeat(int mouse_button)";
		langDef.functions_info["IsMouseButtonUp()"] = "Returns if the mouse button is released. \nbool IsMouseButtonUp(int mouse_button)";
		langDef.functions_info["AngleBetween()_Quat_Static"] = "Returns the angle in degrees between two rotations. \nfloat AngleBetween(TheQuaternion a, TheQuaternion b)";
		langDef.functions_info["DotProduct()_Quat_Static"] = "Returns The dot product between two rotations. \nfloat DotProduct(TheQuaternion a, TheQuaternion b)";
		langDef.functions_info["FromAngleAxis()"] = "Returns a rotation which rotates angle degrees around axis. \nTheQuaternion FromAngleAxis(float angle, TheVector3 axis)";
		langDef.functions_info["FromEulerAngles()"] = "Returns a rotation from euler angles. \nTheQuaternion FromEulerAngles(TheVector3 angles)";
		langDef.functions_info["Identity"] = "The identity rotation. \nTheQuaternion Identity";
		langDef.functions_info["Lerp()_Quat_Static"] = "Linearly interpolates between two rotations. \nTheQuaternion Lerp(TheQuaternion a, TheQuaternion b, float value)";
		langDef.functions_info["LookRotation()"] = "Returns a rotation with the specified direction. \nTheQuaternion LookRotation(TheVector3 direction)";
		langDef.functions_info["Magnitude()_Quat_Static"] = "Returns the Quaternion magnitude. \nfloat Magnitude(TheQuaternion quaternion)";
		langDef.functions_info["Normalize()_Quat_Static"] = "Normalize the Quaternion. \nTheQuaternion Normalize(TheQuaternion quaternion)";
		langDef.functions_info["RotateTowards()"] = "Rotates a rotation from towards to. \nTheQuaternion RotateTowards(TheQuaternion from, TheQuaternion to, float step)";
		langDef.functions_info["Slerp()_Quat_Static"] = "Spherically interpolates between a and b by value. \nTheQuaternion Slerp(TheQuaternion a, TheQuaternion b, float value)";
		langDef.functions_info["ToAngleAxis()_Quat_Static"] = "Returns the quaternion angle and axis. \nvoid ToAngleAxis(TheQuaternion a, out float angle, out TheVector3 axis)";
		langDef.functions_info["Conjugate()"] = "Returns the conjugate of the quaternion. \nTheQuaternion Conjugate()";
		langDef.functions_info["Inverse()"] = "Returns the inverse of the quaternion. \nTheQuaternion Inverse()";
		langDef.functions_info["Length_Quat_NStatic"] = "Returns the length of the quaternion.  \nfloat Length";
		langDef.functions_info["LengthSquared_Quat_NStatic"] = "Returns the length squared of the quaternion. \nfloat LengthSquared";
		langDef.functions_info["Normalized_Quat_NStatic"] = "Returns the quaternion normalized. \nTheQuaternion Normalized";
		langDef.functions_info["Set()_Quat_NStatic"] = "Set the quaternion values. \nvoid Set(float x, float y, float z, float w)";
		langDef.functions_info["ToAngleAxis()_Quat_NStatic"] = "Returns the quaternion angle and axis. \nvoid ToAngleAxis(out float angle, out TheVector3 axis)";
		langDef.functions_info["ToEulerAngles()"] = "Returns the quaternion angle and axis. \nTheVector3 ToEulerAngles()";
		langDef.functions_info["ForwardDirection"] = "The forward direction of the GameObject. \nTheVector3 ForwardDirection";
		langDef.functions_info["GlobalPosition"] = "The global position of the GameObject. \nTheVector3 GlobalPosition";
		langDef.functions_info["GlobalRotation"] = "The global rotation of the GameObject. \nTheVector3 GlobalRotation";
		langDef.functions_info["GlobalScale"] = "The global scale of the GameObject. \nTheVector3 GlobalScale";
		langDef.functions_info["LocalPosition"] = "The local position of the GameObject relative to the parent. \nTheVector3 LocalPosition";
		langDef.functions_info["LocalRotation"] = "The local rotation of the GameObject. \nTheVector3 LocalRotation";
		langDef.functions_info["LocalScale"] = "The local scale of the GameObject. \nTheVector3 LocalScale";
		langDef.functions_info["LookAt()"] = "The local scale of the GameObject. \nvoid LookAt(TheVector3 value)";
		langDef.functions_info["AngleBetween()_Vec_Static"] = "Returns the angle in degrees between a and b. \nfloat AngleBetween(TheVector3 a, TheVector3 b)";
		langDef.functions_info["BackWard"] = "Shorthand for TheVector3(0, 0, -1). \nTheVector3 backward";
		langDef.functions_info["CrossProduct()"] = "Cross Product of two vectors. \nTheVector3 CrossProduct(TheVector3 a, TheVector3 b)";
		langDef.functions_info["Distance()"] = "Returns the distance between a and b. \nfloat Distance(TheVector3 a, TheVector3 b)";
		langDef.functions_info["DotProduct()_Vec_Static"] = "Dot Product of two vectors. \nfloat DotProduct(TheVector3 a, TheVector3 b)";
		langDef.functions_info["Down"] = "Shorthand for TheVector3(0, -1, 0). \nTheVector3 down";
		langDef.functions_info["Forward"] = "Shorthand for TheVector3(0, 0, 1). \nTheVector3 Forward";
		langDef.functions_info["Left"] = "Shorthand for TheVector3(-1, 0, 0). \nTheVector3 Left";
		langDef.functions_info["Lerp()_Vec_Static"] = "Linearly interpolates between two vectors. \nTheVector3 Lerp(TheVector3 start, TheVector3 end, float value)";
		langDef.functions_info["Magnitude()_Vec_Static"] = "Returns the magnitude of the vector. \nfloat Magnitude(TheVector3 vector)";
		langDef.functions_info["MoveTowards()"] = "Moves a point in a straight line towards a target point. \nTheVector3 MoveTowards(TheVector3 position, TheVector3 target, float step)";
		langDef.functions_info["NLerp()"] = "Normalized linear interpolation. Normalize the result of Lerp \nTheVector3 NLerp(TheVector3 start, TheVector3 end, float value)";
		langDef.functions_info["Normalize()_Vec_Static"] = "Normalize the Vector. \nTheVector3 Normalize(TheVector3 vector)";
		langDef.functions_info["Project()"] = "Projects a vector onto another vector. \nTheVector3 Project(TheVector3 vector, TheVector3 normal)";
		langDef.functions_info["Reflect()"] = "Reflects a vector off the plane defined by a normal. \nTheVector3 Reflect(TheVector3 direction, TheVector3 normal)";
		langDef.functions_info["Right"] = "Shorthand for TheVector3(1, 0, 0). \nTheVector3 Right";
		langDef.functions_info["Scale()_Vec_Static"] = "Multiplies two vectors. \nTheVector3 Scale(TheVector3 a, TheVector3 b)";
		langDef.functions_info["Slerp()_Vec_Static"] = "Spherically interpolates between start and end by value. \nTheVector3 SLerp(TheVector3 start, TheVector3 end, float value)";
		langDef.functions_info["Up"] = "Shorthand for TheVector3(0, 1, 0). \nTheVector3 Up";
		langDef.functions_info["Zero"] = "Shorthand for TheVector3(0, 0, 0). \nTheVector3 Zero";
		langDef.functions_info["Length_Vec_NStatic"] = "Returns the length of the vector. \n float Length";
		langDef.functions_info["LengthSquared_Vec_NStatic"] = "Returns the length squared of the vector. \nfloat LengthSquared";
		langDef.functions_info["Normalized_Vec_NStatic"] = "Returns the vector normalized. \nTheVector3 Normalized";
		langDef.functions_info["Scale()_Vec_NStatic"] = "Multiplies the vector by another vector. \nTheVector3 Scale(TheVector3 scale)";
		langDef.functions_info["Set()_Vec_NStatic"] = "Set vector values. \nvoid Set(float x, float y, float z)";
		langDef.functions_info["ToQuaternion()"] = "Converts the vector to a quaternion. \nTheQuaternion ToQuaternion()";
		langDef.functions_info["TimeScale"] = "The scale at which the time is passing. \nfloat Scale";
		langDef.functions_info["DeltaTime"] = "The time in seconds it took to complete the last frame. \nfloat DeltaTime";
		
		variables["Duplicate()"] = "TheGameObject";
		variables["GetChild()"] = "TheGameObject";
		variables["GetComponent<TheTransform>()"] = "TheTransform";
		variables["GetComponent<TheFactory>()"] = "TheFactory";
		variables["AddComponent<TheFactory>()"] = "TheFactory";
		variables["Spawn()"] = "TheGameObject";
		variables["GetMousePosition()"] = "TheVector3";
		variables["Identity"] = "TheQuaternion";
		variables["ToEulerAngles()"] = "TheVector3";
		variables["FromEulerAngles()"] = "TheQuaternion";
		variables["Lerp()_TheQuaternion"] = "TheQuaternion";
		variables["Slerp()_TheQuaternion"] = "TheQuaternion";
		variables["RotateTowards()"] = "TheQuaternion";
		variables["FromAngleAxis()"] = "TheQuaternion";
		variables["LookRotation()"] = "TheQuaternion";
		variables["Normalized_TheQuaternion"] = "TheQuaternion";
		variables["Normalize()_TheQuaternion"] = "TheQuaternion";
		variables["Conjugate()"] = "TheQuaternion";
		variables["Inverse()"] = "TheQuaternion";
		variables["LocalPosition()"] = "TheVector3";
		variables["LocalRotation()"] = "TheVector3";
		variables["LocalScale()"] = "TheVector3";
		variables["GlobalPosition()"] = "TheVector3";
		variables["GlobalRotation()"] = "TheVector3";
		variables["GlobalScale()"] = "TheVector3";
		variables["ForwardDirection"] = "TheVector3";
		variables["Zero"] = "TheVector3";
		variables["Forward"] = "TheVector3";
		variables["BackWard"] = "TheVector3";
		variables["Up"] = "TheVector3";
		variables["Down"] = "TheVector3";
		variables["Left"] = "TheVector3";
		variables["Right"] = "TheVector3";
		variables["Normalized_TheVector3"] = "TheVector3";
		variables["Normalize()_TheVector3"] = "TheVector3";
		variables["Lerp()_TheVector3"] = "TheVector3";
		variables["Slerp()_TheVector3"] = "TheVector3";
		variables["NLerp()"] = "TheVector3";
		variables["Project()"] = "TheVector3";
		variables["MoveTowards()"] = "TheVector3";
		variables["CrossProduct()"] = "TheVector3";
		variables["Reflect()"] = "TheVector3";
		variables["Scale()"] = "TheVector3";
		variables["ToQuaternion()"] = "TheQuaternion";
		
		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;

		langDef.mName = "C#";

		inited = true;
	}
	return langDef;
}