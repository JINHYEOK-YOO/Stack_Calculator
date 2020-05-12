#include <iostream>
#include <stack>
#include <cstring>
#include <cmath>
using namespace std;

bool err_occurred = false;

typedef struct { // 수 또는 문자 표현 구조체
	char symbol; // 숫자, 연산자, 괄호 등의 문자
	int digits = 0; // 숫자의 자릿수 (0 == 연산자나 괄호, 1 == 일의 자릿수, 2 == 십의 자릿수, ...)
} Numeric;

Numeric* transPostfix(char infix[], size_t& postfix_len) // infix notation을 postfix notation으로 변환하는 함수
{
	stack<char> stck;
	Numeric* postfix = new Numeric[1000]; // postfix 식을 담을 배열 동적 할당
	postfix_len = 0; // postfix 식 문자의 길이 (인자로 받은 변수)

	for (size_t i = 0; i < strlen(infix); i++) // infix 식 문자 하나씩 확인
	{
		if (infix[i] == '(') // 왼쪽 괄호일 경우
			stck.push(infix[i]);

		else if (infix[i] >= '0' && infix[i] <= '9') // 숫자일 경우
		{
			postfix[postfix_len].symbol = infix[i]; // postfix 배열에 넣음

			// infix 식의 현재 문자와 이전 문자들을 확인하여 자릿수를 알맞게 증가
			for (size_t iter = 0; (iter <= postfix_len) && (infix[i - iter] >= '0') && (infix[i - iter] <= '9'); iter++)
					postfix[postfix_len - iter].digits++;

			postfix_len++; // postfix 식 문자 길이 증가
		}

		else if (infix[i] == '^') // ^ 연산자일 경우 (가장 높은 우선순위)
		{
			// 스택이 비거나, 스택의 top이 왼쪽 괄호나 낮은 우선순위의 연산자일 때까지 반복
			while (!stck.empty() && stck.top() != '(' && stck.top() != '*' && stck.top() != '/' && stck.top() != '%' && stck.top() != '+' && stck.top() != '-')
			{
				postfix[postfix_len++].symbol = stck.top(); // 스택의 top을 postfix 배열에 넣음
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == '*' || infix[i] == '/' || infix[i] == '%') // * / % 연산자일 경우
		{
			while (!stck.empty() && stck.top() != '(' && stck.top() != '+' && stck.top() != '-')
			{
				postfix[postfix_len++].symbol = stck.top();
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == '+' || infix[i] == '-') // + - 연산자일 경우 (가장 낮은 우선순위)
		{
			while (!stck.empty() && stck.top() != '(') 
			{
				postfix[postfix_len++].symbol = stck.top();
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == ')') // 오른쪽 괄호일 경우
		{
			while (!stck.empty() && stck.top() != '(') // 스택이 비거나 스택의 top이 왼쪽 괄호일 때까지 반복
			{
				postfix[postfix_len++].symbol = stck.top(); // 스택의 top을 postfix 배열에 넣음
				stck.pop();
			}

			if (stck.empty()) // 스택이 비었다면 (오른쪽 괄호가 왼쪽 괄호보다 많을 경우)
			{
				err_occurred = true; // 에러 발생
				return postfix; // 함수 종료
			}

			stck.pop(); // 스택 top에 있는 왼쪽 괄호 pop
		}
	}

	while (!stck.empty()) // 스택에 문자가 남아있는 경우
	{
		if (stck.top() == '(') // 스택 top이 왼쪽 괄호라면 (왼쪽 괄호가 오른쪽 괄호보다 많은 경우)
		{
			err_occurred = true; // 에러 발생
			return postfix;
		}

		postfix[postfix_len++].symbol = stck.top(); // 남은 문자 postfix 배열에 넣음
		stck.pop();
	}

	return postfix;
}

int evalPostfix(Numeric postfix[], size_t len) // postfix 식을 계산하는 함수
{
	stack<int> stck;
	int num = 0; // 문자에서 변환된 수 저장 변수
	int result; // 연산 결과 값 저장 변수

	for (size_t i = 0; i < len; i++) // postfix 식 문자 하나씩 확인
	{
		if (postfix[i].symbol >= '0' && postfix[i].symbol <= '9') // 숫자일 경우
		{
			if (postfix[i].digits == 1) // 일의 자리 수라면
			{
				num += postfix[i].symbol - '0'; // 문자를 해당 정수로 변환 후 
				stck.push(num); // num에 저장된 수를 스택에 push
				num = 0; // 초기화
			}

			else // 십의 자리 이상의 수라면
				num += (postfix[i].symbol - '0') * (int)pow(10, postfix[i].digits - 1); // 자릿수에 맞게 정수 변환 후 값 저장
		}

		else // 연산자일 경우
		{
			int num2 = stck.top();
			stck.pop();
			int num1 = stck.top();
			stck.pop();

			// 각 연산자에 맞게 연산
			switch (postfix[i].symbol)
			{
			case '^':
				result = (int)pow(num1, num2);
				break;

			case '*': 
				result = num1 * num2;
				break;

			case '/':
				if (num2 == 0) // 0으로 나누는 경우
				{
					err_occurred = true; // 에러 발생
					return 0; // 함수 종료
				}
				else
				{
					result = num1 / num2;
					break;
				}

			case '%':
				if (num2 == 0)
				{
					err_occurred = true;
					return 0;
				}
				else
				{
					result = num1 % num2;
					break;
				}

			case '+':
				result = num1 + num2;
				break;

			case '-':
				result = num1 - num2;
				break;
			}

			stck.push(result);
		}
	}

	return stck.top(); // (== return result;)
}

int main()
{
	char infix[1000];
	Numeric* postfix;
	int result;
	size_t postfix_len;

	while (true)
	{
		cin >> infix; // 계산할 식 입력
		
		if (strcmp(infix, "0") == 0) // 0 입력 시 프로그램 종료
		{
			return 0;
		}

		postfix = transPostfix(infix, postfix_len);
		// transPostfix 함수 호출 후 postfix_len 변수에 postfix 식 길이 저장됨

		if (err_occurred) // transPostfix 함수 호출 후 에러 발생했다면
		{
			cout << "Error!" << endl;
			err_occurred = false;
		}
		else
		{
			result = evalPostfix(postfix, postfix_len);

			if (err_occurred) // evalPostfix 함수 호출 후 에러 발생했다면
			{
				cout << "Error!" << endl;
				err_occurred = false;
			}
			else
				cout << result << endl; // 계산 결과 출력
		}

		delete[] postfix; // transPostfix 함수에서 한 동적 할당 해제
	}
}