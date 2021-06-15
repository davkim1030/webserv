#include "libft.h"

/*
 * 입력 받은 문자열을 띄어쓰기, 탭 단위로 나눠서 std::vector로 리턴
 * @param const std::string &str : 띄어쓰기로 나눌 문자열
 * @return std::vector<std::string> & : 문자열을 나눈 결과
 */
std::vector<std::string> splitSpaces(const std::string &str)
{
	std::vector<std::string>	result;
	char	*cur = const_cast<char *>(str.c_str());
	char	*prev = cur;

	// NULL 만날 때까지 반복
	while (*cur)
	{
		if (ft_isblank(*cur))
		{
			result.push_back(str.substr(prev - str.c_str(), cur - prev));
			// *cur가 0이면 멈춰야 함, *cur가 space이면 다음으로 가야함
			while (*cur != '\0' && (*cur == ' ' || *cur == '\t'))
				cur++;
			prev = cur;
		}
		else
			cur++;
	}
	// 루프 다 돌고 나서 안 들어간 값이 있으면 추가
	if (cur != prev)
		result.push_back(str.substr(prev - str.c_str(), cur - prev));
	return (result);
}

