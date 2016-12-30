'use strict'

angular.module('FileBrowser', [ 'ui.bootstrap', 'ngRoute', 'directory' ])

.config(['$locationProvider', '$routeProvider', function($locationProvider, $routeProvider) {
  $locationProvider.html5Mode(true);
  $locationProvider.html5Mode(true);
  
  $routeProvider.when('/phones/:name', {		
		controller : 'directory'
	}).otherwise({
		redirectTo : '/'
	});
}])

.factory('TitleCtrl', function() {
	var title = 'default';
	return {
		title : function() {
			return title;
		},
		setTitle : function(newTitle) {
			title = newTitle
		}
	};
})

.controller('MainCtrl', [ '$scope', '$location', 'TitleCtrl', function ($scope, $location, TitleCtrl) {
	$scope.TitleCtrl = TitleCtrl;
	$scope.TitleCtrl.setTitle(window.location.pathname);
}]);

/*.config([ '$routeProvider', function($routeProvider) {
	$routeProvider.when('/phones/:name', {		
		controller : 'directory'
	}).otherwise({
		redirectTo : '/'
	});
} ]);*/


