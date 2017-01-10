/** Todo Application general implementation class.
 *
 * @namespace
 * */
var TodoApp = {
    /**
     * Contains all views
     * * @namespace
     */
    Views: {},

    /**
     * Contains all models
     * @namespace
     * */
    Models: {},

    /**
     * Collections object
     * @namespace
     * */
    Collections: {}

    /**
     * System routing informations
     * @namespace
     * */,
    Router: {}
};

/**
 * Returnning todo model item. This can be handle CRUD operations itself.
 * @class TodoApp.Models.TodoModel
 * @property {String} idAttribute - Defining Id type
 * @property {String} urlRoot - Http API url
 * @property {Object} defaults - Todo model default parameters configuring
 * */
TodoApp.Models.TodoModel = Backbone.Model.extend({
    idAttribute: 'TodoId',
    urlRoot: '/todos',
    defaults: {
        Todo: ''
    },

    /**
     * Validation executing before save operations.
     * @method TodoApp.Models.TodoModel#validate
     * @memberOf TodoApp.Models.TodoModel
     * @param {object} attrs - TodoModel attributes object for checking.
     * */
    validate: function (attrs) {
        if (!attrs.Todo) {
            return 'Please fill todo.';
        }
    }
});

/**
 * Todo collection for storing all todo models.
 * @class TodoApp.Collections.TodoCollection
 * @property {TodoApp.Models.TodoModel} model - Collections items type
 * */
TodoApp.Collections.TodoCollection = Backbone.Collection.extend({
    model: TodoApp.Models.TodoModel,

    /**
     * Returning API endpoint url.
     * @method TodoApp.Collections.TodoCollection#url
     * @memberOf TodoApp.Collections.TodoCollection
     * @return {string} - API endpoint url.
     * */
    url: function () {
        return "/todos";
    },

    /**
     * Data parse from query response
     * @method TodoApp.Collections.TodoCollection#parse
     * @memberOf TodoApp.Collections.TodoCollection
     * @param {object} response - API request result
     * @return {object} - Model datas
     * */
    parse: function (response) {
        if (response.status)
            return response.data;

        alert(response.message);
        return [];
    }
});


/**
 * Combines UI layer and data layer to manage processes.
 * @class TodoApp.Views.TodoCreate
 * @property {TodoApp.Collections.TodoCollection} model - Item collection
 * @property {object} events - UI element event binding properties
 * */
TodoApp.Views.TodoCreate = Backbone.View.extend({
    model: TodoApp.Collections.TodoCollection,
    events: {
        'click #AddButton': 'addTodo'
    },

    /**
     * Class constructor
     * @constructor
     * @method TodoApp.Views.TodoCreate#initialize
     * @memberOf TodoApp.Views.TodoCreate
     */
    initialize: function () {
        this.template = _.template($('#TodoCreate').html());
    },

    /** Adding todo model to collection. Also firing UI change after adding item.
     * @method TodoApp.Views.TodoCreate#addTodo
     * @memberOf TodoApp.Views.TodoCreate
     */
    addTodo: function () {
        var todoModel = new TodoApp.Models.TodoModel();
        todoModel.set('Todo', this.$el.find('input').val());

        if (todoModel.isValid()) {
            var self = this;

            todoModel.save({}, {
                success: function (data, response) {
                    if (response.status) {
                        self.$el.find('input').val("");
                        todoModel.set("TodoId", response.data.id);
                        self.model.add(todoModel); // Add to TodoCollection
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
            alert(todoModel.validationError);
    },

    /** Rendering template and model datas.
     * @method TodoApp.Views.TodoCreate#render
     * @memberOf TodoApp.Views.TodoCreate
     * @return {object} - Render object
     */
    render: function () {
        this.$el.html(this.template());
        return this;
    }
});

/**
 * Show all todo models.
 * @class TodoApp.Views.TodoList
 * @property {TodoApp.Collections.TodoCollection} model - Item collection
 * @property {string} tagName - Template top level tag type
 * @property {string} tagName - Template top level element class
 * */
TodoApp.Views.TodoList = Backbone.View.extend({
    model: TodoApp.Collections.TodoCollection,
    tagName: 'ul',
    className: 'list-group',

    /**
     * Class constructor
     * @constructor
     * @method TodoApp.Views.TodoList#initialize
     * @memberOf TodoApp.Views.TodoList
     */
    initialize: function (options) {
        if (options.model)
            this.model = options.model;
    },

    /** Rendering template and model datas.
     * @method TodoApp.Views.TodoList#render
     * @memberOf TodoApp.Views.TodoList
     * @return {object} - Render object
     */
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

/**
 * Todo create and todo list container.
 * @class TodoApp.Views.TodoView
 * @property {TodoApp.Collections.TodoCollection} model - Item collection
 * @property {object} events - UI element event binding properties
 * */
TodoApp.Views.TodoView = Backbone.View.extend({
    /**
     * Class constructor
     * @constructor
     * @method TodoApp.Views.TodoView#initialize
     * @memberOf TodoApp.Views.TodoView
     */
    initialize: function () {
        this.todoCollection = new TodoApp.Collections.TodoCollection();
        this.todoCollection.fetch({async: false});

        this.todoCollection.on('add', this.addOne, this);
    },

    /** Rendering template and model datas.
     * @method TodoApp.Views.TodoView#render
     * @memberOf TodoApp.Views.TodoView
     * @return {object} - Render object
     */
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

    /** Todo collection adding callback. Rendering new item and adding to UI.
     * @method TodoApp.Views.TodoView#render
     * @memberOf TodoApp.Views.TodoView
     */
    addOne: function (todo) {
        var todoItemView = new TodoApp.Views.TodoItem({model: todo});
        this.$el.find(".list-group").prepend(todoItemView.render().el);
    }
});

/**
 * Todo create and todo list container.
 * @class TodoApp.Views.TodoItem
 * @property {TodoApp.Models.TodoItem} model - Item collection
 * @property {string} tagName - Template top level tag type
 * @property {string} tagName - Template top level element class
 * */
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

    /** Rendering template and model datas.
     * @method TodoApp.Views.TodoItem#render
     * @memberOf TodoApp.Views.TodoItem
     * @return {object} - Render object
     */
    render: function () {
        this.$el.html(this.template(this.model.attributes));
        return this;
    },

    /** Removing from UI.
     * @method TodoApp.Views.TodoItem#remove
     * @memberOf TodoApp.Views.TodoItem
     */
    remove: function () {
        this.$el.remove();
    },

    /** UI element delete method.
     * @method TodoApp.Views.TodoItem#remove
     * @memberOf TodoApp.Views.TodoItem
     */
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

    /** UI element edit method.
     * @method TodoApp.Views.TodoItem#edit
     * @memberOf TodoApp.Views.TodoItem
     */
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

    /** Inverse edit mode and preapre UI elements.
     * @method TodoApp.Views.TodoItem#__setEditMode
     * @memberOf TodoApp.Views.remove
     */
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
    },
});

/**
 * Todo create and todo list container.
 * @class TodoApp.Router
 * @property {object} routes - Routing roules
 * @property {function} todoview - TodoView execution function
 * */
TodoApp.Router = Backbone.Router.extend({

    routes: {
        '*path': 'todoview'
    },

    todoview: function () {
        var view = new TodoApp.Views.TodoView();
        $('#main').html(view.render().el);
    }
});

/**
 * JQuery auto execute after page load completed
 * */
$(document).ready(function () {
    TodoApp.Router.Instance = new TodoApp.Router();
    Backbone.history.start();
});