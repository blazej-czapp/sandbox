#!/usr/local/bin/python
# -*- coding: utf-8 -*-
E = -1
O = 0
X = 1
BOARD_SIZE = 3
NO_OF_RANKS = BOARD_SIZE * 2 + 2
INF = float(400000)
import sys

class Board(object):
    def __init__(self, copy = None):
        if copy == None:
            self.board = [[E for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]
        else:
            self.board = []
            for row in copy.board:
                new_row = []
                self.board.append(new_row)
                for col in row:
                    new_row.append(col)

    def consider_move(self, move):
        ''' Returns a new board which is the result of playing this move '''
        assert self.board[move.where[0]][move.where[1]] == E, "attempt to play on a non-empty field"
        parallel_universe = Board(self)
        parallel_universe.board[move.where[0]][move.where[1]] = move.what
        return parallel_universe

    def generate_possible_moves(self, player):
        moves = []
        for (i, row) in enumerate(self.board):
            for (j, col) in enumerate(row):
                if col == E:
                    moves.append(Move(player, [i, j]))

        return moves

    def has_empty_fields(self):
        for row in self.board:
            for col in row:
                if col == E:
                    return False
        return True

    def has_line_for(self, player):
        for row in self.board:
            if all(col == player for col in row):
                return True

        for col in range(BOARD_SIZE):
            if all(row[col] == player for row in self.board):
                return True

        if all(self.board[i][i] == player for i in range(BOARD_SIZE)):
            return True

        if all(self.board[i][BOARD_SIZE - i - 1] == player for i in range(BOARD_SIZE)):
            return True

        return False

    def score_for_rank(self, rank, player):
        if any(field == next_player(player) for field in rank):
            return 0
        else:
            count = sum(map(lambda x: 1 if x == player else 0, rank))
            if count == BOARD_SIZE - 1: # one short of winning position
                #return INF / NO_OF_RANKS / 2
                return 0
            if count == BOARD_SIZE: # a winning position
                return INF

            return 0# 2 ** count

    def calculate_score_for(self, player):
        total = 0.0
        for row in self.board:
            total += self.score_for_rank(row, player)

        for col in range(BOARD_SIZE):
            column = [row[col] for row in self.board]
            total += self.score_for_rank(column, player)

        diag1 = [self.board[i][i] for i in range(BOARD_SIZE)]
        total += self.score_for_rank(diag1, player)

        diag2 = [self.board[i][BOARD_SIZE - i - 1] for i in range(BOARD_SIZE)]
        total += self.score_for_rank(diag2, player)

        return total

    def score(self):
        scoreX = self.calculate_score_for(X)
        scoreO = self.calculate_score_for(O)
        return scoreX - scoreO

    def __repr__(self):
        ''' prints the state of the board '''
        result = ""
        for (i, row) in enumerate(self.board):
            for (j, col) in enumerate(row):
                result += symbol_to_string(col) + ("│" if j < len(row) - 1 else "")
            
            result += "\n"
            for (j, col) in enumerate(row):
                result += "─┼" if j < len(row) - 1 and i < len(self.board) - 1 else ""
            result += "─\n" if i < len(self.board) - 1 else ""
        return result

class Move(object):
    def __init__(self, what, where):
        self.what = what
        self.where = where

def symbol_to_string(symbol):
    if symbol == O:
        return "O"
    elif symbol == X:
        return "X"
    else:
        return " "

def next_player(current_player):
    return X if current_player == O else O

def play(board, player, bestX, bestO, depth):
    global total
    total += 1
    moves = board.generate_possible_moves(player)

    if len(moves) == 1:
        return (board.consider_move(moves[0]).score(), moves[0])

    best_move = None
    for move in moves:
        consequence = board.consider_move(move)
        if player == X:
            if consequence.score() >= INF / 2: # a winning move, stop right here
                return (consequence.score(), move)
        else:
            if consequence.score() <= -INF / 2: # a winning move, stop right here
                return (consequence.score(), move)

        if depth > 0:
            score, _ = play(consequence, next_player(player), bestX, bestO, depth - 1)
        else:
            score = consequence.score()

        if player == X:
            if score >= bestO or score == 1:
                return (score, move)
            if score > bestX:
                bestX = score
                best_move = move
        else:
            if score <= bestX or score == -1:
                return (score, move)
            if score < bestO:
                bestO = score
                best_move = move

    if player == X:
        return (bestX, best_move)
    else:
        return (bestO, best_move)


test_board = Board()
total = 0

print("You're playing O's")
while True:
    _, move = play(test_board, X, -2, 2, 8)
    print(total)
    test_board = test_board.consider_move(move)
    print(test_board)
    user_input = input("Your move>> ")
    if user_input == "q":
        break
    choice = user_input.split(" ")
    test_board = test_board.consider_move(Move(O, [int(choice[0]), int(choice[1])]))
