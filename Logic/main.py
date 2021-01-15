# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import random
from random import choice


class TrigEquation(str):
    func = ''
    a = 0
    t = 'x'
    k = 1
    m = 0
    eq = ''

    def __init__(self, eq_type):
        super().__init__()
        self.func = choice(('sin', 'cos', 'tg', 'ctg'))
        self.eq = eq_type.replace('T', self.func, 1)

        if self.func == 'sin' or 'cos':
            r = random.random()
            print(r)
            if r < 0.4:
                self.a = choice(list(trig['sin']['table'].keys()))
                self.eq = self.eq.replace('a', self.a, 1)
                print(self.a)
            elif r < 0.7:
                self.a = round(random.uniform(0, 1), 1)
                self.eq = self.eq.replace('a', str(self.a), 1)
        else:
            r = random.random()
            if r < 0.5:
                self.a = choice(list(trig['tg']['table'].keys()))
                self.eq = self.eq.replace('a', self.a, 1)
                print(self.a)

        for char in eq_type:
            if char.isalpha():
                if char != 'x':
                    num = round(random.uniform(0, 10), 1)
                    self.eq = self.eq.replace(char, str(num), 1)

        self.t = self.eq[self.eq.index('(') + 1:self.eq.index(')')]
        if 'k' in eq_type:
            self.k = '' + self.t[:self.t.index('x')]
        if '+' in self.t:
            self.m = '' + self.t[self.t.index('+') + 1:]
        elif '-' in self.t:
            self.m = '' + self.t[self.t.index('-') + 1:]
        self.a = self.eq[self.eq.index('=') + 1:]
        if self.a == '1.0':
            self.a = '1'

    def solve(self):
        if ((self.func == 'sin') or (self.func == 'cos')) and not (self.a in trig['sin']['table'].keys()):
            if abs(float(self.a)) > 1:
                print('Шаг 1: Решения нет')
        else:
            print('Шаг 1: Решение есть')
            hard = False
            if len(self.t) > 1:
                hard = True
                print('Шаг 2: Аргумент сложный')
                print('Введём новую переменную: t = ' + self.t)
            else:
                print('Шаг 2: Аргумент простой - x')
            print('Шаг 3: Формула решения')
            sol = 0
            if self.a in trig[self.func]['table']:
                sol = trig[self.func]['table'][self.a]
                print('Значение табличное')
                print(self.t + ' = ' + sol)
            else:
                sol = trig[self.func]['sol'].replace('x', self.a, 1)
                print('Значение нетабличное')
                print(self.t + ' = ' + sol)
            if hard:
                print('Шаг 4: Упрощение выражения')
                if self.m != 0:
                    sol = '-' + self.m + ' + ' + sol
                    print('x = ' + sol)
                if self.k != 1:
                    print('x = (' + sol + ')/' + self.k)


trig = {'sin': {'sol': '(-1)^n * arcsin(x) + pi*n',
                'table': {'0': 'pi/2', '1/2': 'pi/3', 'sqrt(2)/2': 'pi/4', 'sqrt(3)/2': 'pi/6', '1': '0'}},
        'cos': {'sol': '+/-arccos(x) + 2pi*n',
                'table': {'0': '0', '1/2': 'pi/6', 'sqrt(2)/2': 'pi/4', 'sqrt(3)/2': 'pi/6', '1': 'pi/2'}},
        'tg': {'sol': 'arctg(x) + pi*n',
               'table': {'0': '0', '1/sqrt(3)': 'pi/6', 'sqrt(3)': 'pi/3', '1': 'pi/4'}},
        'ctg': {'sol': 'arcctg(x) + pi*n',
                'table': {'0': 'pi/2', '1/sqrt(3)': 'pi/3', 'sqrt(3)': 'pi/6', '1': 'pi/4'}}}

if __name__ == '__main__':
    e1 = TrigEquation('T(x)=a')
    print(e1.eq)
    e1.solve()

