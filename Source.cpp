#include <iostream>
#include <stack>
#include <cstring>
#include <cmath>
using namespace std;

bool err_occurred = false;

typedef struct { // �� �Ǵ� ���� ǥ�� ����ü
	char symbol; // ����, ������, ��ȣ ���� ����
	int digits = 0; // ������ �ڸ��� (0 == �����ڳ� ��ȣ, 1 == ���� �ڸ���, 2 == ���� �ڸ���, ...)
} Numeric;

Numeric* transPostfix(char infix[], size_t& postfix_len) // infix notation�� postfix notation���� ��ȯ�ϴ� �Լ�
{
	stack<char> stck;
	Numeric* postfix = new Numeric[1000]; // postfix ���� ���� �迭 ���� �Ҵ�
	postfix_len = 0; // postfix �� ������ ���� (���ڷ� ���� ����)

	for (size_t i = 0; i < strlen(infix); i++) // infix �� ���� �ϳ��� Ȯ��
	{
		if (infix[i] == '(') // ���� ��ȣ�� ���
			stck.push(infix[i]);

		else if (infix[i] >= '0' && infix[i] <= '9') // ������ ���
		{
			postfix[postfix_len].symbol = infix[i]; // postfix �迭�� ����

			// infix ���� ���� ���ڿ� ���� ���ڵ��� Ȯ���Ͽ� �ڸ����� �˸°� ����
			for (size_t iter = 0; (iter <= postfix_len) && (infix[i - iter] >= '0') && (infix[i - iter] <= '9'); iter++)
					postfix[postfix_len - iter].digits++;

			postfix_len++; // postfix �� ���� ���� ����
		}

		else if (infix[i] == '^') // ^ �������� ��� (���� ���� �켱����)
		{
			// ������ ��ų�, ������ top�� ���� ��ȣ�� ���� �켱������ �������� ������ �ݺ�
			while (!stck.empty() && stck.top() != '(' && stck.top() != '*' && stck.top() != '/' && stck.top() != '%' && stck.top() != '+' && stck.top() != '-')
			{
				postfix[postfix_len++].symbol = stck.top(); // ������ top�� postfix �迭�� ����
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == '*' || infix[i] == '/' || infix[i] == '%') // * / % �������� ���
		{
			while (!stck.empty() && stck.top() != '(' && stck.top() != '+' && stck.top() != '-')
			{
				postfix[postfix_len++].symbol = stck.top();
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == '+' || infix[i] == '-') // + - �������� ��� (���� ���� �켱����)
		{
			while (!stck.empty() && stck.top() != '(') 
			{
				postfix[postfix_len++].symbol = stck.top();
				stck.pop();
			}
			stck.push(infix[i]);
		}

		else if (infix[i] == ')') // ������ ��ȣ�� ���
		{
			while (!stck.empty() && stck.top() != '(') // ������ ��ų� ������ top�� ���� ��ȣ�� ������ �ݺ�
			{
				postfix[postfix_len++].symbol = stck.top(); // ������ top�� postfix �迭�� ����
				stck.pop();
			}

			if (stck.empty()) // ������ ����ٸ� (������ ��ȣ�� ���� ��ȣ���� ���� ���)
			{
				err_occurred = true; // ���� �߻�
				return postfix; // �Լ� ����
			}

			stck.pop(); // ���� top�� �ִ� ���� ��ȣ pop
		}
	}

	while (!stck.empty()) // ���ÿ� ���ڰ� �����ִ� ���
	{
		if (stck.top() == '(') // ���� top�� ���� ��ȣ��� (���� ��ȣ�� ������ ��ȣ���� ���� ���)
		{
			err_occurred = true; // ���� �߻�
			return postfix;
		}

		postfix[postfix_len++].symbol = stck.top(); // ���� ���� postfix �迭�� ����
		stck.pop();
	}

	return postfix;
}

int evalPostfix(Numeric postfix[], size_t len) // postfix ���� ����ϴ� �Լ�
{
	stack<int> stck;
	int num = 0; // ���ڿ��� ��ȯ�� �� ���� ����
	int result; // ���� ��� �� ���� ����

	for (size_t i = 0; i < len; i++) // postfix �� ���� �ϳ��� Ȯ��
	{
		if (postfix[i].symbol >= '0' && postfix[i].symbol <= '9') // ������ ���
		{
			if (postfix[i].digits == 1) // ���� �ڸ� �����
			{
				num += postfix[i].symbol - '0'; // ���ڸ� �ش� ������ ��ȯ �� 
				stck.push(num); // num�� ����� ���� ���ÿ� push
				num = 0; // �ʱ�ȭ
			}

			else // ���� �ڸ� �̻��� �����
				num += (postfix[i].symbol - '0') * (int)pow(10, postfix[i].digits - 1); // �ڸ����� �°� ���� ��ȯ �� �� ����
		}

		else // �������� ���
		{
			int num2 = stck.top();
			stck.pop();
			int num1 = stck.top();
			stck.pop();

			// �� �����ڿ� �°� ����
			switch (postfix[i].symbol)
			{
			case '^':
				result = (int)pow(num1, num2);
				break;

			case '*': 
				result = num1 * num2;
				break;

			case '/':
				if (num2 == 0) // 0���� ������ ���
				{
					err_occurred = true; // ���� �߻�
					return 0; // �Լ� ����
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
		cin >> infix; // ����� �� �Է�
		
		if (strcmp(infix, "0") == 0) // 0 �Է� �� ���α׷� ����
		{
			return 0;
		}

		postfix = transPostfix(infix, postfix_len);
		// transPostfix �Լ� ȣ�� �� postfix_len ������ postfix �� ���� �����

		if (err_occurred) // transPostfix �Լ� ȣ�� �� ���� �߻��ߴٸ�
		{
			cout << "Error!" << endl;
			err_occurred = false;
		}
		else
		{
			result = evalPostfix(postfix, postfix_len);

			if (err_occurred) // evalPostfix �Լ� ȣ�� �� ���� �߻��ߴٸ�
			{
				cout << "Error!" << endl;
				err_occurred = false;
			}
			else
				cout << result << endl; // ��� ��� ���
		}

		delete[] postfix; // transPostfix �Լ����� �� ���� �Ҵ� ����
	}
}