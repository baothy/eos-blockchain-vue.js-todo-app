<template>
  <div id="app">

    <div class="container mx-auto">
      <div class="mx-auto w-full md:w-1/2 py-8 px-4">
        
        <!-- Title -->
        <div class="flex items-center mb-6">
          <img src="/src/assets/eos-logo.png" class="hidden md:block" width="100" alt="">
          <h1 class="tracking-wide text-white mr-6">EOS Blockchain - Vue.JS <br> <span class="">TODO List</span></h1>          
        </div>

        <!-- New Todo Input -->
        <div class="input-wrapper relative">
          <input type="text"
                v-model="newTodo"
                v-on:keyup.enter="addTodo"
                v-on:focus="show_guide = true"
                v-on:blur="show_guide = false"
                placeholder="Enter your task here"
                class="p-4 mb-4 w-full bg-transparent border-grey-light text-white border rounded">

          <transition name="fade">
            <span class="text-guide text-grey-darker absolute text-xs" v-show="show_guide">Press Enter</span>       
          </transition>   
        </div>
      
        <!-- To Do List -->    
        <ul class="list-reset">
          <transition-group name="fade">
            <li v-for="todo in todos" :key="todo.id"
                class="py-6 px-2 border-b border-grey-darkest flex justify-between items-center relative todo__item">
              <div>
                <input type="checkbox" :id="todo.id" class="cbx hidden" v-model="todo.isDone" v-on:click="updateTodo(todo)">
                <label :for="todo.id" class="text-xl cbx__child"></label>
                <label :for="todo.id" class="cbx__lbl text-white inline-block mt-1" :class="{ completed: todo.isDone }">{{ todo.content }}</label>
              </div>
              <button v-on:click="removeTodo(todo)" type="button" class="flex items-center delete-button absolute pin-r">
                <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-x"><line x1="18" y1="6" x2="6" y2="18"></line><line x1="6" y1="6" x2="18" y2="18"></line></svg>
              </button>
            </li>          
          </transition-group>
        </ul>
      </div>
    </div>
  </div>
</template>

<script>
const Eos = require("eosjs");
const config = require("./config");
const STORAGE_KEY = "todo-storage";

let contrat = null;

const eos = Eos.Localnet({
  keyProvider: config.eos.keyProvider,
  httpEndpoint: config.eos.httpEndpoint
});

export default {
  name: "app",
  data() {
    return {
      newTodo: "",
      todos: [],
      show_guide: false
    };
  },

  methods: {
    async getTodos() {
      const table = await eos.getTableRows({
        scope: config.eos.contract.scope,
        code: config.eos.contract.code,
        table: config.eos.contract.table,
        json: true
      });

      this.todos = table.rows;
    },

    async addTodo() {
      if (this.newTodo.length) {
        const content = this.newTodo;
        const result = await contrat.create(
          config.eos.contract.scope,
          content,
          {
            authorization: [config.eos.contract.authorization]
          }
        );

        this.getTodos();
      }
      this.newTodo = "";
    },
    async removeTodo(todo) {
      const result = await contrat.remove(todo.id, config.eos.contract.scope, {
        authorization: [config.eos.contract.authorization]
      });
      this.getTodos();
    },
    async updateTodo(todo) {
      const content = this.newTodo;
      let isDone = todo.isDone === 1 ? 0 : 1;
      const result = await contrat.update(
        config.eos.contract.scope,
        todo.content,
        todo.id,
        isDone,
        {
          authorization: [config.eos.contract.authorization]
        }
      );
    }
  },

  async mounted() {
    contrat = await eos.contract(config.eos.contract.code);
    this.getTodos();
  },

  watch: {
    todos: {
      handler() {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(this.todos));
      },
      deep: true
    }
  }
};
</script>

<style lang="scss">
$c-primary: #cecece;

// Global
body {
  font-family: "Raleway", sans-serif;
}

@keyframes octocat-wave {
  0%,
  100% {
    transform: rotate(0);
  }
  20%,
  60% {
    transform: rotate(-25deg);
  }
  40%,
  80% {
    transform: rotate(10deg);
  }
}

// Title
h1 {
  span {
    color: #606f7b;
    font-size: 25px;
  }
}

// Input
.input-wrapper {
  input {
    transition: border-color 0.3s ease;
    border: none;
    border-bottom: 1px solid;

    &:focus {
      outline: none;
      border-color: $c-primary;
    }
  }

  .text-guide {
    right: 0;
    bottom: -8px;
    text-transform: uppercase;
  }
}

// Checkbox & Label
.cbx__lbl {
  margin-left: 4px;
  vertical-align: middle;
  cursor: pointer;
  position: relative;

  &:after {
    content: "";
    width: 0%;
    height: 2px;
    background: $c-primary;
    position: absolute;
    left: 0;
    top: 50%;
    display: block;
    transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
  }

  &.completed {
    color: #c8ccd4;
    &:after {
      width: 100%;
    }
  }
}

.cbx:checked ~ .cbx__child {
  border-color: transparent;
  background: $c-primary;
  animation: jelly 0.6s ease;
}

.cbx:checked ~ .cbx__child:after {
  opacity: 1;
  transform: rotate(45deg) scale(1);
}

@keyframes jelly {
  from {
    transform: scale(1, 1);
  }
  30% {
    transform: scale(1.25, 0.75);
  }
  40% {
    transform: scale(0.75, 1.25);
  }
  50% {
    transform: scale(1.15, 0.85);
  }
  65% {
    transform: scale(0.95, 1.05);
  }
  75% {
    transform: scale(1.05, 0.95);
  }
  to {
    transform: scale(1, 1);
  }
}

.cbx__child {
  position: relative;
  top: 1px;
  width: 17px;
  height: 17px;
  margin-right: 20px;
  border: 1px solid #c8ccd4;
  border-radius: 3px;
  vertical-align: middle;
  display: inline-block;
  transition: background 0.1s ease;
  cursor: pointer;

  &:after {
    content: "";
    position: absolute;
    top: 1px;
    left: 5px;
    width: 5px;
    height: 11px;
    opacity: 0;
    transform: rotate(45deg) scale(0);
    border-right: 2px solid #fff;
    border-bottom: 2px solid #fff;
    transition: all 0.3s ease;
    transition-delay: 0.15s;
  }
}

// Fade In/Out Animation
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.3s;
}

.fade-enter,
.fade-leave-to {
  opacity: 0;
}

// Todo List
.todo {
  &__item {
    &:hover {
      .delete-button {
        opacity: 1;
        visibility: visible;
      }
    }
    &:last-child {
      border-bottom: none;
    }
  }
}

// Delete Button
.delete-button {
  opacity: 0;
  visibility: hidden;
  transition: opacity 0.3s ease;

  svg {
    transition: all 0.3s ease;
    height: 20px;
  }

  .feather {
    color: #fff;
  }
  &:hover .feather {
    color: #ff6b6b;
  }

  &:focus {
    outline: none;
    border: 1px dashed #ff6b6b;
  }
}
</style>
