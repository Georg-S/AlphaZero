#include "GameHandling/TrainingUtility.h"

std::pair<std::string, int> getHighestExistingNetAndIteration(const std::string& path)
{
	std::string res = "";
	int highestExistingNet = -1;
	std::smatch baseMatch;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		auto pathString = entry.path().string();
		if (std::regex_search(pathString, baseMatch, std::regex("\\d+")))
		{
			int iteration = std::stoi(baseMatch[0]);
			if (iteration > highestExistingNet)
			{
				res = pathString;
				highestExistingNet = iteration;
			}
		}
	}

	return { res, highestExistingNet };
}