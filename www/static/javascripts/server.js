angular.module('todoApp', [])
    .controller('TodoListController', function($http) {
        var todoList = this;
        todoList.todos = [];

        todoList.addTodo = function() {
            $http.get("/api/search?term=" + todoList.todoText)
                .then(function(response) {
                todoList.todos = response.data.data;
            });

        };

        todoList.remaining = function() {
            var count = 0;
            angular.forEach(todoList.todos, function(todo) {
                count += todo.done ? 0 : 1;
            });
            return count;
        };

        todoList.archive = function() {
            var oldTodos = todoList.todos;
            todoList.todos = [];
            angular.forEach(oldTodos, function(todo) {
                if (!todo.done) todoList.todos.push(todo);
            });
        };
    });