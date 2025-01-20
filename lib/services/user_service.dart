class UserService {
  String? currentUser;

  void login(String username, String password) {
    // Simuler une connexion
    currentUser = username;
    print('User $username logged in');
  }

  void logout() {
    print('User $currentUser logged out');
    currentUser = null;
  }

  String? getCurrentUser() {
    return currentUser;
  }
}
