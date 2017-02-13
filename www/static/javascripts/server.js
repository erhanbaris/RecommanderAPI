var TodoApp = {
    Views: {},
    Models: {},
    Collections: {},
    Router: {}
};

TodoApp.Models.TodoModel = Backbone.Model.extend({
    idAttribute: 'Id',
    defaults: {
        Name: ''
    },

    validate: function (attrs) {
        if (!attrs.Name) {
            return 'Please fill todo.';
        }
    }
});

TodoApp.Collections.TodoCollection = Backbone.Collection.extend({
    model: TodoApp.Models.TodoModel,

    parse: function (response) {
        if (response.status)
            return response.data;

        return [];
    }
});

TodoApp.Views.TodoCreate = Backbone.View.extend({
    model: TodoApp.Collections.TodoCollection,
    events: {
        'click #SearchButton': 'search'
    },

    initialize: function () {
        this.template = _.template($('#TodoCreate').html());
    },

    search: function () {
        var todoModel = new TodoApp.Models.TodoModel();
        todoModel.set('Name', this.$el.find('input').val());

        if (todoModel.isValid()) {
            var self = this;
            self.model.reset();

            Backbone.ajax({
                url: "/api/search?term=" + this.$el.find('input').val(),
                data: "",
                success: function(val){
                    self.model.add(val);
                }
            });
        }
        else
            alert(todoModel.validationError);
    },

    render: function () {
        this.$el.html(this.template());
        return this;
    }
});

TodoApp.Views.TodoList = Backbone.View.extend({
    model: TodoApp.Collections.TodoCollection,
    tagName: 'ul',
    className: 'list-group',

    initialize: function (options) {
        if (options.model)
            this.model = options.model;
    },

    render: function () {
        this.$el.html();

        // Add all todo item to UI
        for (var i = 0; i < this.model.length; ++i) {
            var todoItem = new TodoApp.Views.TodoItem({model: this.model.at(i)});
            this.$el.append(todoItem.$el);
            todoItem.render();
        }

        return this;
    }
});

TodoApp.Views.TodoView = Backbone.View.extend({

    initialize: function () {
        this.todoCollection = new TodoApp.Collections.TodoCollection();
        this.todoCollection.on('add', this.addOne, this);
    },

    render: function () {

        this.$el.html(); // lets render this view

        var self = this;

        var createTodo = new TodoApp.Views.TodoCreate({model: this.todoCollection});
        var todoList = new TodoApp.Views.TodoList({model: this.todoCollection});

        this.$el.append(createTodo.$el);
        this.$el.append(todoList.$el);

        createTodo.render();
        todoList.render();

        return this;
    },

    addOne: function (todo) {
        var todoItemView = new TodoApp.Views.TodoItem({model: todo});
        this.$el.find(".list-group").prepend(todoItemView.render().el);
    }
});

TodoApp.Views.TodoItem = Backbone.View.extend({
    model: TodoApp.Models.TodoModel,
    tagName: "li",
    className: "list-group-item",

    initialize: function (options) {
        this.template = _.template($("#TodoItem").html());

        this.editing = false;

        this.model.on('destroy', this.remove, this);

        if (options.model)
            this.model = options.model;
    },

    render: function () {
        this.$el.html(this.template(this.model.attributes));
        return this;
    },

    remove: function () {
        this.$el.remove();
    },

    delete: function () {
        var self = this;
        if (this.editing)
        {
            this.__setEditMode(!this.editing);
            self.$el.find("input").val(this.model.get('Todo'))
        }
        else if (confirm("Do you want to delete this todo?") == true)
            this.model.destroy({}, {
                success: function (data, response) {
                    if (!response.status)
                        alert(response.message);
                },
                error: function (data) {
                    console.log('error');
                }
            });
    },

    edit: function () {
        var self = this;
        if (this.editing) {
            this.model.set("Todo", self.$el.find("input").val());
            this.model.save({}, {
                success: function (data, response) {
                    if (response.status) // Check update status
                    {
                        self.__setEditMode(!self.editing);
                        self.render(); // render for changing
                    }
                    else
                        alert(response.message);
                },
                error: function (data) {
                    console.log('error');
                }
            });
        }
        else
            this.__setEditMode(!this.editing);
    },

    __setEditMode: function (status) {
        if (!this.editing) {
            this.$el.find(".todo").removeClass("show");
            this.$el.find("input").removeClass("hidden");

            this.$el.find(".todo").addClass("hidden");
            this.$el.find("input").removeClass("show");

            this.$el.find(".edit").html("Save");
            this.$el.find(".delete").html("Cancel");
        }
        else {
            this.$el.find(".todo").removeClass("hidden");
            this.$el.find("input").removeClass("show");

            this.$el.find(".todo").addClass("show");
            this.$el.find("input").addClass("hidden");

            this.$el.find(".edit").html("Edit");
            this.$el.find(".delete").html("Delete");
        }

        this.editing = !this.editing; // inverse editing parameter
    },

    events: {
        'click .edit': 'edit',
        'click .delete': 'delete'
    }
});

TodoApp.Router = Backbone.Router.extend({

    routes: {
        '*path': 'todoview'
    },

    todoview: function () {
        var view = new TodoApp.Views.TodoView();
        $('#main').html(view.render().el);
    }
});

$(document).ready(function () {
    TodoApp.Router.Instance = new TodoApp.Router();
    Backbone.history.start();
});